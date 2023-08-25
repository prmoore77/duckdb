#include "duckdb/storage/statistics/column_statistics.hpp"
#include "duckdb/common/serializer.hpp"
#include "duckdb/common/serializer/format_deserializer.hpp"
#include "duckdb/common/serializer/format_serializer.hpp"

namespace duckdb {

ColumnStatistics::ColumnStatistics(BaseStatistics stats_p) : stats(std::move(stats_p)) {
	if (DistinctStatistics::TypeIsSupported(stats.GetType())) {
		distinct_stats = make_uniq<DistinctStatistics>();
	}
}
ColumnStatistics::ColumnStatistics(BaseStatistics stats_p, unique_ptr<DistinctStatistics> distinct_stats_p)
    : stats(std::move(stats_p)), distinct_stats(std::move(distinct_stats_p)) {
}

shared_ptr<ColumnStatistics> ColumnStatistics::CreateEmptyStats(const LogicalType &type) {
	return make_shared<ColumnStatistics>(BaseStatistics::CreateEmpty(type));
}

void ColumnStatistics::Merge(ColumnStatistics &other) {
	stats.Merge(other.stats);
	if (distinct_stats) {
		distinct_stats->Merge(*other.distinct_stats);
	}
}

BaseStatistics &ColumnStatistics::Statistics() {
	return stats;
}

bool ColumnStatistics::HasDistinctStats() {
	return distinct_stats.get();
}

DistinctStatistics &ColumnStatistics::DistinctStats() {
	if (!distinct_stats) {
		throw InternalException("DistinctStats called without distinct_stats");
	}
	return *distinct_stats;
}

void ColumnStatistics::SetDistinct(unique_ptr<DistinctStatistics> distinct) {
	this->distinct_stats = std::move(distinct);
}

void ColumnStatistics::UpdateDistinctStatistics(Vector &v, idx_t count) {
	if (!distinct_stats) {
		return;
	}
	auto &d_stats = (DistinctStatistics &)*distinct_stats;
	d_stats.Update(v, count);
}

shared_ptr<ColumnStatistics> ColumnStatistics::Copy() const {
	return make_shared<ColumnStatistics>(stats.Copy(), distinct_stats ? distinct_stats->Copy() : nullptr);
}
void ColumnStatistics::Serialize(Serializer &serializer) const {
	stats.Serialize(serializer);
	serializer.WriteOptional(distinct_stats);
}

shared_ptr<ColumnStatistics> ColumnStatistics::Deserialize(Deserializer &source, const LogicalType &type) {
	auto stats = BaseStatistics::Deserialize(source, type);
	auto distinct_stats = source.ReadOptional<DistinctStatistics>();
	return make_shared<ColumnStatistics>(stats.Copy(), std::move(distinct_stats));
}

void ColumnStatistics::FormatSerialize(FormatSerializer &serializer) const {
	serializer.WriteProperty(100, "statistics", stats);
	serializer.WritePropertyWithDefault(101, "distinct", distinct_stats, unique_ptr<DistinctStatistics>());
}

static shared_ptr<ColumnStatistics> FormatDeserialize(FormatDeserializer &deserializer) {
	// TODO: do we read this as an property or into the object itself?
	// we have this sort of pseudo inheritance going on here which is annoying
	auto stats = BaseStatistics::FormatDeserialize(deserializer);
	auto distinct_stats = deserializer.ReadPropertyWithDefault<unique_ptr<DistinctStatistics>>(
	    101, "distinct", unique_ptr<DistinctStatistics>());
	return make_shared<ColumnStatistics>(stats.Copy(), std::move(distinct_stats));
}

} // namespace duckdb
