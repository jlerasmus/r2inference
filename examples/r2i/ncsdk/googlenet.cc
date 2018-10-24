/* Copyright (C) 2018 RidgeRun, LLC (http://www.ridgerun.com)
 * All Rights Reserved.
 *
 * The contents of this software are proprietary and confidential to RidgeRun,
 * LLC.  No part of this program may be photocopied, reproduced or translated
 * into another programming language without prior written consent of
 * RidgeRun, LLC.  The user is free to modify the source code after obtaining
 * a software license from RidgeRun.  All source code changes must be provided
 * back to RidgeRun without any encumbrance.
*/

#include <getopt.h>
#include <iostream>
#include <memory>
#include <string>

#include <r2i/r2i.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define GOOGLENET_DIM 224

const float GoogleNetMean[] = {0.40787054 * 255.0,
                               0.45752458 * 255.0,
                               0.48109378 * 255.0
                              };

void PrintTopPrediction (std::shared_ptr<r2i::IPrediction> prediction) {
  r2i::RuntimeError error;
  int index = 0;
  double max = -1;

  int num_labels = prediction->GetResultSize();

  for (int i = 0; i < num_labels; ++i) {
    double current = prediction->At(i, error);
    if (current > max) {
      max = current;
      index = i;
    }
  }

  std::cout << "Highest probability is label " << index <<
            " (" << max << ")" << std::endl;
}

void PrintUsage() {
  std::cerr << "Usage: example -i [JPG input_image] -m [GoogLeNet Model]" <<
            std::endl;
}

std::unique_ptr<float[]> PreProcessImage (const unsigned char *input,
    int width, int height, int reqwidth,
    int reqheight, const float *mean) {
  const int channels = 3;
  const int scaled_size = channels * reqwidth * reqheight;

  std::unique_ptr<unsigned char[]> scaled (new unsigned char[scaled_size]);
  std::unique_ptr<float[]> adjusted (new float[scaled_size]);

  stbir_resize_uint8(input, width, height, 0, scaled.get(), reqwidth,
                     reqheight, 0, channels);

  for (int i = 0; i < scaled_size; i += channels) {
    // BGR = RGB - Mean
    adjusted[i + 0] = static_cast<float>(scaled[i + 2]) - mean[0];
    adjusted[i + 1] = static_cast<float>(scaled[i + 1]) - mean[1];
    adjusted[i + 2] = static_cast<float>(scaled[i + 0]) - mean[2];
  }

  return adjusted;
}

std::unique_ptr<float[]> LoadImage(const std::string &path, int reqwidth,
                                   int reqheight, const float *mean) {
  int channels = 3;
  int width, height, cp;

  unsigned char *img = stbi_load(path.c_str(), &width, &height, &cp, channels);
  if (!img) {
    std::cerr << "The picture " << path << " could not be loaded";
    return nullptr;
  }

  auto ret = PreProcessImage(img, width, height, reqwidth, reqheight, mean);
  free (img);

  return ret;
}

bool ParseArgs (int &argc, char *argv[], std::string &image_path,
                std::string &model_path, int &index) {
  int option = 0;

  while ((option = getopt(argc, argv, "i:m:p:")) != -1) {
    switch (option) {
      case 'i' :
        image_path = optarg;
        break;
      case 'm' :
        model_path  = optarg;
        break;
      case 'p' :
        index  = std::stoi (optarg);
        break;
      default:
        return false;
    }
  }

  return true;
}


int main (int argc, char *argv[]) {
  r2i::RuntimeError error;
  std::string model_path;
  std::string image_path;
  int Index = 0;

  if (false == ParseArgs (argc, argv, image_path, model_path, Index)) {
    PrintUsage ();
    exit (EXIT_FAILURE);
  }

  if (image_path.empty() || model_path.empty ()) {
    PrintUsage ();
    exit (EXIT_FAILURE);
  }

  auto factory = r2i::IFrameworkFactory::MakeFactory(r2i::FrameworkCode::NCSDK,
                 error);

  std::cout << "Loading Model: " << model_path << "..." << std::endl;
  auto loader = factory->MakeLoader (error);
  auto model = loader->Load (model_path, error);
  if (error.IsError ()) {
    std::cerr << "Loader error: " << error << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Setting model to engine..." << std::endl;
  auto engine = factory->MakeEngine (error);
  error = engine->SetModel (model);

  std::cout << "Loading image: " << image_path << "..." << std::endl;
  std::unique_ptr<float[]> image_data = LoadImage (image_path, GOOGLENET_DIM,
                                        GOOGLENET_DIM, GoogleNetMean);

  std::cout << "Configuring frame..." << std::endl;
  std::shared_ptr<r2i::IFrame> frame = factory->MakeFrame (error);
  error = frame->Configure (image_data.get(), GOOGLENET_DIM, GOOGLENET_DIM,
                            r2i::ImageFormat::Id::RGB);

  std::cout << "Starting engine..." << std::endl;
  error = engine->Start ();
  if (error.IsError ()) {
    std::cerr << "Engine start error: " << error << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Predicting..." << std::endl;
  auto prediction = engine->Predict (frame, error);
  if (error.IsError ()) {
    std::cerr << "Engine prediction error: " << error << std::endl;
    exit(EXIT_FAILURE);
  }

  PrintTopPrediction (prediction);

  std::cout << "Stopping engine..." << std::endl;
  error = engine->Stop ();
  if (error.IsError ()) {
    std::cerr << "Engine stop error: " << error << std::endl;
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}