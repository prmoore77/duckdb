//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb_python/import_cache/modules/numpy_module.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb_python/import_cache/python_import_cache_item.hpp"

namespace duckdb {

struct NumpyCacheItem : public PythonImportCacheItem {
public:
	static constexpr const char *Name = "numpy";

public:
	~NumpyCacheItem() override {
	}
	virtual void LoadSubtypes(PythonImportCache &cache) override {
		ndarray.LoadAttribute("ndarray", cache, *this);
		datetime64.LoadAttribute("datetime64", cache, *this);
		int64.LoadAttribute("int64", cache, *this);
	}

public:
	PythonImportCacheItem ndarray;
	PythonImportCacheItem datetime64;
	PythonImportCacheItem int64;
};

} // namespace duckdb
