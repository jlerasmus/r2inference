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

#ifndef R2I_TENSORFLOW_MODEL_H
#define R2I_TENSORFLOW_MODEL_H

#include <memory>
#include <tensorflow/c/c_api.h>

#include <r2i/imodel.h>
#include <r2i/runtimeerror.h>

namespace r2i {
namespace tensorflow {

class Model : public IModel {
 public:
  Model ();

  RuntimeError Start (const std::string &name) override;

  std::shared_ptr<TF_Graph> GetGraph ();
  std::shared_ptr<TF_Buffer> GetBuffer ();
  TF_Operation *GetInputOperation ();
  TF_Operation *GetOutputOperation ();
  RuntimeError SetInputLayerName (const std::string &name);
  RuntimeError SetOutputLayerName (const std::string &name);
  const std::string GetInputLayerName ();
  const std::string GetOutputLayerName ();

  RuntimeError Load (std::shared_ptr<TF_Buffer> buffer);

 private:
  std::shared_ptr<TF_Graph> graph;
  std::shared_ptr<TF_Buffer> buffer;
  TF_Operation *in_operation;
  TF_Operation *out_operation;
  std::string input_layer_name;
  std::string output_layer_name;
};

}
}

#endif //R2I_TENSORFLOW_MODEL_H
