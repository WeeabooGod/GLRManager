// Portions of this code Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in Chromium's LICENSE file.
#pragma once
#include <Ultralight/private/util/Debug.h>

#define GENERATE_DESTRUCTOR_ERRMSG(T) "Ref-counted object '#T' was deleted without calling Release()"

namespace ultralight {

template <class T>
class RefCountedImpl {
public:
  RefCountedImpl()
    : ref_count_(1)
#ifndef NDEBUG
    , in_destructor_(false)
#endif
  {
  }

  virtual ~RefCountedImpl() {
#ifndef NDEBUG
    UL_DCHECK(in_destructor_, GENERATE_DESTRUCTOR_ERRMSG(T));
#endif
  }

  virtual void AddRef() const {
#ifndef NDEBUG
    UL_DCHECK(!in_destructor_);
#endif
    ++ref_count_;
  }

  virtual void Release() const {
#ifndef NDEBUG
    UL_DCHECK(!in_destructor_);
#endif
    if (--ref_count_ == 0) {
#ifndef NDEBUG
      in_destructor_ = true;
#endif
      delete static_cast<const T*>(this);
    }
  }

  virtual int ref_count() const {
    return ref_count_;
  }

private:
  mutable int ref_count_;
#ifndef NDEBUG
  mutable bool in_destructor_;
#endif
};

// A macro to disallow the copy constructor and operator= functions.
// This should be used in the private: declarations for a class.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete

}  // namespace ultralight
