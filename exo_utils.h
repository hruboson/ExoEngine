#pragma once

#include <functional>

namespace exo {

	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	struct UploadContext {
		VkFence _uploadFence;
		VkCommandPool _commandPool;
	};

	UploadContext _uploadContext;

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

}