#pragma once

#include <vector>
#include <utility>
#include <iostream>

namespace jaszyk {
	template <class TKey, class TValue>
	class flatmap {
	public:
		using value_type = std::pair<const TKey, TValue>;
		using reference = value_type&;
		using const_reference = const value_type&;
		using iterator = typename std::vector<value_type>::iterator;
		using const_iterator = typename std::vector<value_type>::const_iterator;

		inline flatmap() = default;
		inline flatmap(const flatmap& other) = default;
		inline flatmap(flatmap&& other) noexcept = default;

		inline flatmap& operator=(const flatmap& other) = default;
		inline flatmap& operator=(flatmap&& other) noexcept = default;

		inline void insert(const TKey& key, const TValue& val) {
			data_.emplace_back(key, val);
		}

		inline void insert(const TKey& key, TValue&& val) {
			data_.emplace_back(key, std::move(val));
		}

		inline void insert(TKey&& key, TValue&& val) {
			data_.emplace_back(std::move(key), std::move(val));
		}

		template <class ...TArgs>
		inline void emplace(const TKey& key, TArgs&&... vals) {
			data_.emplace_back(key, std::forward<TArgs>(vals)...);
		}

		inline bool contains(const TKey& key) const {
			for (const auto& [k, v] : data_) {
				if (key == k) return true;
			}
			return false;
		}

		inline TValue& operator[](const TKey& key) {
			for (auto& p : data_) {
				if (p.first == key) {
					return p.second;
				}
			}
			data_.emplace_back(key, TValue());
			return data_.back().second;
		}

		inline const TValue& operator[](const TKey& key) const {
			for (auto& p : data_) {
				if (p.first == key) {
					return p.second;
				}
			}

			throw std::runtime_error("Key does not exist!");
			return data_.end()->second;
		}

		inline size_t size() const {
			return data_.size();
		}

		inline bool empty() const {
			return data_.empty();
		}

		inline void clear() {
			data_.clear();
		}

		inline iterator begin() {
			return data_.begin();
		}

		inline iterator end() {
			return data_.end();
		}

		inline const_iterator begin() const {
			return data_.begin();
		}

		inline const_iterator end() const {
			return data_.end();
		}

	private:
		std::vector<value_type> data_;
	};
}