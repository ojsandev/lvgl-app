#pragma once
#include "common/types.h"
#include "lv/core/objectref.hpp"
#include "lv/core/object.hpp"
#include "lv/widgets/label.hpp"

namespace app::ui::utils {
using ObjectFlag = lv_obj_flag_t;

inline void enableEventBubble(const lv::ObjectRef& object)
{
  if (object == nullptr) {
    return;
  }

  const auto childCount =
      static_cast<common::types::Int32>(object.child_count());

  for (common::types::Int32 i = 0; i < childCount; ++i) {

    if (auto child = lv::ObjectRef(object.child(i)); !child.check_type(lv::Label::class_ptr())) {
      child.add_flag(LV_OBJ_FLAG_EVENT_BUBBLE);
      enableEventBubble(child);
    }
  }
}

inline void clearFlagRecursive(const lv::ObjectRef& object, const ObjectFlag flag)
{
  if (object == nullptr) {
    return;
  }

  lv_obj_remove_flag(object.get(), flag);

  const auto childCount = static_cast<common::types::Int32>(object.child_count());

  for (common::types::Int32 i = 0; i < childCount; ++i) {
    clearFlagRecursive(lv::ObjectRef(object.child(i)), flag);
  }
}

}
