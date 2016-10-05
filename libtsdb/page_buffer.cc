/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include "page_buffer.h"
#include "varint.h"

namespace tsdb {

PageBuffer::PageBuffer(PageType type) : type_(type) {
  switch (type_) {
    case PageType::UINT64:
      new (values_) ValueVectorUInt64Type();
      break;
  }
}

PageBuffer::PageBuffer(
    PageBuffer&& o) :
    type_(o.type_),
    timestamps_(std::move(o.timestamps_)) {
  switch (type_) {
    case PageType::UINT64:
      new (values_) ValueVectorUInt64Type(
          std::move(*((ValueVectorUInt64Type*) o.values_)));
      break;
  }
}

PageBuffer& PageBuffer::operator=(const PageBuffer& o) {
  assert(type_ == o.type_);
  timestamps_ = o.timestamps_;
  switch (type_) {
    case PageType::UINT64:
      *((ValueVectorUInt64Type*) values_) =
          *((ValueVectorUInt64Type*) o.values_);
      break;
  }

  return *this;
}

PageBuffer& PageBuffer::operator=(PageBuffer&& o) {
  assert(type_ == o.type_);
  timestamps_ = std::move(o.timestamps_);
  switch (type_) {
    case PageType::UINT64:
      *((ValueVectorUInt64Type*) values_) =
          std::move(*((ValueVectorUInt64Type*) o.values_));
      break;
  }

  return *this;
}

PageBuffer::~PageBuffer() {
  switch (type_) {
    case PageType::UINT64:
      ((ValueVectorUInt64Type*) values_)->~vector();
      break;
  }
}

template <typename ValueVectorType, typename ValueType>
static void insertValue(
    ValueVectorType* value_vector,
    size_t pos,
    const ValueType& value) {
  value_vector->insert(value_vector->begin() + pos, value);
}

void PageBuffer::insert(uint64_t time, const void* value, size_t value_len) {
  auto timestamp_iter = std::lower_bound(
      timestamps_.begin(),
      timestamps_.end(),
      time);

  auto pos = timestamp_iter - timestamps_.begin();
  timestamps_.insert(timestamp_iter, time);

  switch (type_) {
    case PageType::UINT64:
      insertValue((ValueVectorUInt64Type*) values_, pos, *((uint64_t*) value));
      break;
  }
}

void PageBuffer::getTimestamp(size_t pos, uint64_t* timestamp) const {
  assert(pos < timestamps_.size());
  *timestamp = timestamps_[pos];
}

void PageBuffer::getValue(size_t pos, uint64_t* value) const {
  assert(type_ == PageType::UINT64);
  auto& values = *((ValueVectorUInt64Type*) values_);
  assert(pos < values.size());
  *value = values[pos];
}

size_t PageBuffer::getSize() const {
  return timestamps_.size();
}

void PageBuffer::encode(std::string* out) const {
  writeVarUInt(out, timestamps_.size());
  for (const auto& t : timestamps_) {
    writeVarUInt(out, t);
  }
  auto& values = *((ValueVectorUInt64Type*) values_);
  for (const auto& v : values) {
    writeVarUInt(out, v);
  }
}

} // namespace tsdb

