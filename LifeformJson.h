/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_LIFEFORM_JSON_H_
#define EVOL_LIFEFORM_JSON_H_

#include <json-c/json.h>
#include <unistd.h>

#include <memory>
#include <string>
#include <utility>

namespace evol {


class JsonDeleter {
 public:
  void operator()(json_object *p) {
    // Decrement json-c library reference count
    json_object_put(p);
  }
};


/**
 * Serializes iterators to the given container of Lifeform pointers into JSON.
 * Returns a unique_ptr to the json_object array.
 */
template<typename Iter>
std::unique_ptr<json_object, JsonDeleter> JsonifyLifeforms(const Iter & begin_it, const Iter &end_it) {
  std::unique_ptr<json_object, JsonDeleter> json_lifeform_array (json_object_new_array(), JsonDeleter());

  for (auto it = begin_it; it != end_it; it++) {
    Lifeform * lf = *it;
    struct json_object *json_lifeform = json_object_new_object();
    json_object_object_add(json_lifeform, "id", json_object_new_int64(lf->Id()));
    json_object_object_add(json_lifeform, "gen", json_object_new_int64(lf->Gen()));
    json_object_object_add(json_lifeform, "alive", json_object_new_boolean(lf->Alive()));
    json_object_object_add(json_lifeform, "energy", json_object_new_double(lf->GetEnergy()));

    json_object *dna = json_object_new_array();
    for (auto & opcode : lf->GetDna()) {
      std::string opcode_name;
      auto iter = kOpcodeStrings.find(opcode);
      if (iter != kOpcodeStrings.end()) {
        opcode_name = iter->second;
      } else {
        opcode_name = "?UNKNOWN?";
      }
      json_object_array_add(dna, json_object_new_string(opcode_name.c_str()));
    }
    json_object_object_add(json_lifeform, "dna", dna);
    json_object_array_add(json_lifeform_array.get(), json_lifeform);
  }

  return json_lifeform_array;
}


}  // namespace evol
#endif  // EVOL_LIFEFORM_JSON_H_
