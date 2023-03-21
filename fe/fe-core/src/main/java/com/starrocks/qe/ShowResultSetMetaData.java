// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

package com.starrocks.qe;

// Meta data to describe result set of show statement.
// Because ResultSetMetaData is complicated, redefine it.

import com.google.common.base.Preconditions;
import com.google.common.collect.Lists;
import com.starrocks.catalog.Column;
import com.starrocks.catalog.ScalarType;
import com.starrocks.sql.common.ErrorType;
import com.starrocks.sql.common.StarRocksPlannerException;

import java.util.List;

public class ShowResultSetMetaData {
    private List<Column> columns;

    public int getColumnCount() {
        return columns.size();
    }

    public List<Column> getColumns() {
        return columns;
    }

    public Column getColumn(int idx) {
        return columns.get(idx);
    }

    public int getColumnIdx(String colName) {
        for (int idx = 0; idx < columns.size(); ++idx) {
            if (columns.get(idx).getName().equalsIgnoreCase(colName)) {
                return idx;
            }
        }
        throw new StarRocksPlannerException("Can't get column " + colName, ErrorType.INTERNAL_ERROR);
    }

    public void removeColumn(int idx) {
        Preconditions.checkArgument(idx < columns.size());
        columns.remove(idx);
    }

    public ShowResultSetMetaData(List<Column> columns) {
        this.columns = columns;
    }

    public static Builder builder() {
        return new Builder();
    }

    public static class Builder {
        private List<Column> columns;

        public Builder() {
            columns = Lists.newArrayList();
        }

        public ShowResultSetMetaData build() {
            return new ShowResultSetMetaData(columns);
        }

        public Builder addColumn(Column col) {
            columns.add(col);
            return this;
        }
        public Builder column(String name, ScalarType type) {
            columns.add(new Column(name, type, false, null, true, null, ""));
            return this;
        }

    }
}
