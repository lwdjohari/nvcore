/*
 *  Copyright (c) 2024 Linggawasistha Djohari
 * <linggawasistha.djohari@outlook.com> Licensed to Linggawasistha Djohari under
 * one or more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 *
 *  Linggawasistha Djohari licenses this file to you under the Apache License,
 *  Version 2.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef NVM_CORE_CONTAINERS_V2_RECORD_PAGE_H
#define NVM_CORE_CONTAINERS_V2_RECORD_PAGE_H

#include <cmath>
#include <cstdint>
namespace nvm::containers {

struct Page {
  size_t page;
  size_t item_per_page;
  bool is_paging;

  Page() : page(1), item_per_page(30), is_paging(true){};

  explicit Page(size_t page, size_t item_per_page = 30, bool is_paging = true)
      : page(page), item_per_page(item_per_page), is_paging(is_paging) {}

}

class RecordPage final {
 private:
  size_t total_items_;
  uint32_t items_per_page_;
  size_t total_pages_;

 public:
  /// Constructor initializing total items and items per page
  explicit RecordPage(size_t total_items, uint32_t items_per_page)
      : total_items_(total_items),
        items_per_page_(items_per_page),
        total_pages_(
            std::ceil(static_cast<double>(total_items_) / items_per_page_)) {}

  /// Returns the total number of pages
  size_t Pages() const { return total_pages_; }

  /// Calculates the starting offset for the requested page
  size_t GetPageOffset(size_t page) const {
    if (page < 1 || page > total_pages_) {
      return 0;
    }
    return (page - 1) * items_per_page_;
  }

  /// Accessor for total items
  size_t TotalItems() const { return total_items_; }

  /// Accessor for items per page
  int32_t ItemsPerPage() const { return items_per_page_; }
};
}  // namespace nvm::containers
#endif