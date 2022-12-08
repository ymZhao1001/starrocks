// Copyright 2021-present StarRocks, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "common/status.h"
#include "gen_cpp/parquet_types.h"
#include "runtime/types.h"

namespace starrocks::parquet {

struct LevelInfo {
    int16_t max_def_level = 0;
    int16_t max_rep_level = 0;

    int16_t immediate_repeated_ancestor_def_level = 0;

    bool is_nullable() const { return max_def_level > immediate_repeated_ancestor_def_level; }

    int16_t increment_repeated() {
        auto origin_ancestor_rep_levels = immediate_repeated_ancestor_def_level;
        max_def_level++;
        max_rep_level++;
        immediate_repeated_ancestor_def_level = max_def_level;
        return origin_ancestor_rep_levels;
    }

    std::string debug_string() const;
};

struct ParquetField {
    std::string name;
    tparquet::SchemaElement schema_element;

    // Used to identify if this field is a nested field.
    TypeDescriptor type;
    bool is_nullable;

    // Only valid when this field is a leaf node
    tparquet::Type::type physical_type;
    // If type is FIXED_LEN_BYTE_ARRAY, this is the byte length of the vales.
    int32_t type_length;

    // Used when this column contains decimal data.
    int32_t scale;
    int32_t precision;

    // used to get ColumnChunk in parquet file's metadata
    int physical_column_index;

    LevelInfo level_info;
    std::vector<ParquetField> children;

    int16_t max_def_level() const { return level_info.max_def_level; }
    int16_t max_rep_level() const { return level_info.max_rep_level; }
    std::string debug_string() const;
};

class SchemaDescriptor {
public:
    SchemaDescriptor() = default;
    ~SchemaDescriptor() = default;

    Status from_thrift(const std::vector<tparquet::SchemaElement>& t_schemas, bool case_sensitive);

    std::string debug_string() const;

    int get_column_index(const std::string& column) const;
    const ParquetField* get_stored_column_by_idx(int idx) const { return &_fields[idx]; }
    const ParquetField* resolve_by_name(const std::string& name) const;
    void get_field_names(std::unordered_set<std::string>* names) const;

private:
    void leaf_to_field(const tparquet::SchemaElement& t_schema, const LevelInfo& cur_level_info, bool is_nullable,
                       ParquetField* field);

    Status list_to_field(const std::vector<tparquet::SchemaElement>& t_schemas, size_t pos, LevelInfo cur_level_info,
                         ParquetField* field, size_t* next_pos);

    Status map_to_field(const std::vector<tparquet::SchemaElement>& t_schemas, size_t pos, LevelInfo cur_level_info,
                        ParquetField* field, size_t* next_pos);

    Status group_to_struct_field(const std::vector<tparquet::SchemaElement>& t_schemas, size_t pos,
                                 LevelInfo cur_level_info, ParquetField* field, size_t* next_pos);

    Status group_to_field(const std::vector<tparquet::SchemaElement>& t_schemas, size_t pos, LevelInfo cur_level_info,
                          ParquetField* field, size_t* next_pos);

    Status node_to_field(const std::vector<tparquet::SchemaElement>& t_schemas, size_t pos, LevelInfo cur_level_info,
                         ParquetField* field, size_t* next_pos);

    std::vector<ParquetField> _fields;
    std::vector<ParquetField*> _physical_fields;
    std::unordered_map<std::string, int> _field_idx_by_name;

    bool _case_sensitive = false;
};

template <typename T>
class ColumnStats {
public:
private:
    T _min_value;
    T _max_value;
};

} // namespace starrocks::parquet
