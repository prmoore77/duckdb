import pytest
import tempfile

import os

_ = pytest.importorskip("duckdb.experimental.spark")

from duckdb.experimental.spark.sql import SparkSession as session
from duckdb import connect


@pytest.fixture
def df(spark):
    simpleData = (
        ("Java", 4000, 5),
        ("Python", 4600, 10),
        ("Scala", 4100, 15),
        ("Scala", 4500, 15),
        ("PHP", 3000, 20),
    )
    columns = ["CourseName", "fee", "discount"]
    dataframe = spark.createDataFrame(data=simpleData, schema=columns)
    yield dataframe


class TestSparkToParquet(object):
    def test_basic_to_parquet(self, df):
        temp_file_name = os.path.join(tempfile.mkdtemp(), next(tempfile._get_candidate_names()))

        spark_session = session.builder.getOrCreate()

        df.write.parquet(temp_file_name)

        csv_rel = spark_session.read.parquet(temp_file_name)

        assert df.collect() == csv_rel.collect()
    
    def test_compressed_to_parquet(self, df):
        temp_file_name = os.path.join(tempfile.mkdtemp(), next(tempfile._get_candidate_names()))

        spark_session = session.builder.getOrCreate()

        df.write.parquet(temp_file_name, compression = "ZSTD")

        csv_rel = spark_session.read.parquet(temp_file_name)

        assert df.collect() == csv_rel.collect()
