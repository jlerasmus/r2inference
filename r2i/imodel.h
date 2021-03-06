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

#ifndef R2I_IMODEL_H
#define R2I_IMODEL_H

#include <r2i/runtimeerror.h>

/**
 * R2Inference Namespace
 */
namespace r2i {
/**
 * Implements the interface to abstract a framework model
 */
class IModel {
 public:
  /**
   * \brief Initializes a model with a name.
   * \param name A string with the name of the model.
   * \return error RuntimeError with a description of an error.
   */

  virtual RuntimeError Start (const std::string &name) = 0;

  /**
   * \brief Default destructor
   */
  virtual ~IModel () {};
};

}

#endif // R2I_IMODEL_H
