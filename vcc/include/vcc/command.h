/*
 * Copyright 2016 Google Inc. All Rights Reserved.

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 * http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _VCC_COMMAND_H_
#define _VCC_COMMAND_H_

#include <array>
#include <vcc/command_buffer.h>
#include <vcc/descriptor_set.h>
#include <vcc/event.h>
#include <vcc/input_buffer.h>
#include <vcc/pipeline.h>
#include <vcc/query_pool.h>

namespace vcc {
namespace command {

namespace internal {


template<typename BuildT>
command_buffer::command_buffer_type &get_command_buffer(BuildT &build) {
	return *build.command_buffer;
}

template<typename BuildT>
auto get_pre_execute_callbacks(BuildT &build)->decltype(build.pre_execute_callbacks)& {
	return build.pre_execute_callbacks;
}

template<typename BuildT>
auto get_references(BuildT &build)->decltype(build.references)& {
	return build.references;
}

}  // namespace internal

struct build_type {
	template<typename BuildT>
	friend command_buffer::command_buffer_type &internal::get_command_buffer(BuildT &build);
	template<typename BuildT>
	friend auto internal::get_pre_execute_callbacks(BuildT &build)
		->decltype(build.pre_execute_callbacks)&;
	template<typename BuildT>
	friend auto internal::get_references(BuildT &build)->decltype(build.references)&;

	build_type() = default;
	build_type(const build_type &) = delete;
	build_type(build_type &&) = default;
	build_type &operator=(const build_type &) = delete;
	build_type &operator=(build_type &&) = default;

	VCC_LIBRARY ~build_type();

	explicit build_type(const type::supplier<command_buffer::command_buffer_type> &command_buffer);
private:
	type::supplier<command_buffer::command_buffer_type> command_buffer;
	std::unique_lock<std::mutex> command_buffer_lock;
	vcc::internal::hook_container_type<const queue::queue_type&> pre_execute_callbacks;
	vcc::internal::reference_container_type references;
};

VCC_LIBRARY build_type build(
	const type::supplier<command_buffer::command_buffer_type> &command_buffer,
	VkCommandBufferUsageFlags flags,
	const type::supplier<const render_pass::render_pass_type> &render_pass,
	uint32_t subpass,
	const type::supplier<const framebuffer::framebuffer_type> &framebuffer,
	VkBool32 occlusionQueryEnable, VkQueryControlFlags queryFlags,
	VkQueryPipelineStatisticFlags pipelineStatistics);

VCC_LIBRARY build_type build(
	const type::supplier<command_buffer::command_buffer_type> &command_buffer,
	VkCommandBufferUsageFlags flags,
	VkBool32 occlusionQueryEnable, VkQueryControlFlags queryFlags,
	VkQueryPipelineStatisticFlags pipelineStatistics);

struct bind_pipeline {
	VkPipelineBindPoint pipelineBindPoint;
	type::supplier<const pipeline::pipeline_type> pipeline;
};

struct set_viewport {
	uint32_t first_viewport;
	std::vector<VkViewport> viewports;
};

struct set_scissor {
	uint32_t first_scissor;
	std::vector<VkRect2D> scissors;
};

struct set_line_width {
	float lineWidth;
};

struct set_depth_bias {
	float depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor;
};

struct set_blend_constants {
	std::array<float, 4> blendConstants;
};

struct set_depth_bounds {
	float minDepthBounds, maxDepthBounds;
};

struct set_stencil_compare_mask {
	VkStencilFaceFlags faceMask;
	uint32_t compareMask;
};

struct set_stencil_write_mask {
	VkStencilFaceFlags faceMask;
	uint32_t writeMask;
};

struct set_stencil_reference {
	VkStencilFaceFlags faceMask;
	uint32_t reference;
};

struct bind_descriptor_sets {
	VkPipelineBindPoint pipelineBindPoint;
	type::supplier<const pipeline_layout::pipeline_layout_type> layout;
	uint32_t firstSet;
	std::vector<type::supplier<const descriptor_set::descriptor_set_type>>
		descriptor_sets;
	std::vector<uint32_t> dynamic_offsets;
};

struct bind_index_buffer_type {
	type::supplier<const buffer::buffer_type> buffer;
	VkDeviceSize offset;
	VkIndexType indexType;
};

inline bind_index_buffer_type bind_index_buffer(
		const type::supplier<const buffer::buffer_type> &buffer,
		VkDeviceSize offset, VkIndexType indexType) {
	return { buffer, offset, indexType };
}

struct bind_index_data_buffer_type {
	type::supplier<const input_buffer::input_buffer_type> buffer;
	VkDeviceSize offset;
	VkIndexType indexType;
};

inline bind_index_data_buffer_type bind_index_data_buffer(
	const type::supplier<const input_buffer::input_buffer_type> &buffer,
	VkDeviceSize offset, VkIndexType indexType) {
	return { buffer, offset, indexType };
}

struct bind_vertex_buffers_type {
	uint32_t first_binding;
	std::vector<type::supplier<const buffer::buffer_type>> buffers;
	std::vector<VkDeviceSize> offsets;
};

inline bind_vertex_buffers_type bind_vertex_buffers(uint32_t first_binding,
	const std::initializer_list<type::supplier<const buffer::buffer_type>> &buffers,
	const std::vector<VkDeviceSize> &offsets) {
	return{ first_binding, buffers, offsets };
}

inline bind_vertex_buffers_type bind_vertex_buffers(uint32_t first_binding,
	const std::vector<type::supplier<const buffer::buffer_type>> &buffers,
	const std::vector<VkDeviceSize> &offsets) {
	return { first_binding, buffers, offsets };
}

struct bind_vertex_data_buffers_type {
	uint32_t first_binding;
	std::vector<type::supplier<const input_buffer::input_buffer_type>> buffers;
	std::vector<VkDeviceSize> offsets;
};

inline bind_vertex_data_buffers_type bind_vertex_buffers(uint32_t first_binding,
		const std::vector<type::supplier<const input_buffer::input_buffer_type>> &buffers,
		const std::vector<VkDeviceSize> &offsets) {
	return { first_binding, buffers, offsets };
}

struct draw {
	uint32_t vertexCount, instanceCount, firstVertex, firstInstance;
};

struct draw_indexed {
	uint32_t indexCount, instanceCount, firstIndex, vertexOffset,
		firstInstance;
};

struct draw_indirect_type {
	type::supplier<const buffer::buffer_type> buffer;
	VkDeviceSize offset;
	uint32_t drawCount, stride;
};

inline draw_indirect_type draw_indirect(
		const type::supplier<const buffer::buffer_type> &buffer,
		VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
	return { buffer, offset, drawCount, stride };
}

struct draw_indirect_data_type {
	type::supplier<const input_buffer::input_buffer_type> buffer;
	VkDeviceSize offset;
	uint32_t drawCount, stride;
};

inline draw_indirect_data_type draw_indirect(
		const type::supplier<const input_buffer::input_buffer_type> &buffer,
		VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
	return { buffer, offset, drawCount, stride };
}

struct draw_indexed_indirect_type {
	type::supplier<const buffer::buffer_type> buffer;
	VkDeviceSize offset;
	uint32_t drawCount, stride;
};

inline draw_indexed_indirect_type draw_indexed_indirect(
		const type::supplier<const buffer::buffer_type> &buffer,
		VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
	return { buffer, offset, drawCount, stride };
}

struct draw_indexed_indirect_data_type {
	type::supplier<const input_buffer::input_buffer_type> buffer;
	VkDeviceSize offset;
	uint32_t drawCount, stride;
};

inline draw_indexed_indirect_data_type draw_indexed_indirect_data(
		const type::supplier<const input_buffer::input_buffer_type> &buffer,
		VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
	return { buffer, offset, drawCount, stride };
}

struct dispatch {
	uint32_t x, y, z;
};

struct dispatch_indirect_type {
	type::supplier<const buffer::buffer_type> buffer;
	VkDeviceSize offset;
};

inline dispatch_indirect_type dispatch_indirect(
		const type::supplier<const buffer::buffer_type> &buffer, VkDeviceSize offset) {
	return { buffer, offset };
}

struct dispatch_indirect_data_type {
	type::supplier<const input_buffer::input_buffer_type> buffer;
	VkDeviceSize offset;
};

inline dispatch_indirect_data_type dispatch_indirect_data(
		const type::supplier<const input_buffer::input_buffer_type> &buffer,
		VkDeviceSize offset) {
	return { buffer, offset };
}

struct copy_buffer_type {
	type::supplier<const buffer::buffer_type> srcBuffer, dstBuffer;
	std::vector<VkBufferCopy> regions;
};

inline copy_buffer_type copy_buffer(
		const type::supplier<const buffer::buffer_type> &srcBuffer,
		type::supplier<const buffer::buffer_type> &&dstBuffer,
		const std::vector<VkBufferCopy> &regions) {
	return { srcBuffer, dstBuffer, regions };
}

struct copy_data_buffer_type {
	type::supplier<const input_buffer::input_buffer_type> srcBuffer;
	type::supplier<const buffer::buffer_type> dstBuffer;
	std::vector<VkBufferCopy> regions;
};

inline copy_data_buffer_type copy_data_buffer(
		const type::supplier<const input_buffer::input_buffer_type> &srcBuffer,
		const type::supplier<const buffer::buffer_type> &dstBuffer,
		const std::vector<VkBufferCopy> &regions) {
	return { srcBuffer, dstBuffer, regions };
}

struct copy_image {
	type::supplier<const image::image_type> srcImage;
	VkImageLayout srcImageLayout;
	type::supplier<const image::image_type> dstImage;
	VkImageLayout dstImageLayout;
	std::vector<VkImageCopy> regions;
};

struct blit_image {
	type::supplier<const image::image_type> srcImage;
	VkImageLayout srcImageLayout;
	type::supplier<const image::image_type> dstImage;
	VkImageLayout dstImageLayout;
	std::vector<VkImageBlit> regions;
	VkFilter filter;
};

struct copy_buffer_to_image_type {
	type::supplier<const buffer::buffer_type> srcBuffer;
	type::supplier<const image::image_type> dstImage;
	VkImageLayout dstImageLayout;
	std::vector<VkBufferImageCopy> regions;
};

inline copy_buffer_to_image_type copy_buffer_to_image(
		const type::supplier<const buffer::buffer_type> &srcBuffer,
		const type::supplier<const image::image_type> &dstImage,
		VkImageLayout dstImageLayout,
		const std::vector<VkBufferImageCopy> &regions) {
	return { srcBuffer, dstImage, dstImageLayout, regions };
}

struct copy_data_buffer_to_image_type {
	type::supplier<const input_buffer::input_buffer_type> srcBuffer;
	type::supplier<const image::image_type> dstImage;
	VkImageLayout dstImageLayout;
	std::vector<VkBufferImageCopy> regions;
};

inline copy_data_buffer_to_image_type copy_data_buffer_to_image(
		const type::supplier<const input_buffer::input_buffer_type> &srcBuffer,
		const type::supplier<const image::image_type> &dstImage,
		VkImageLayout dstImageLayout, const std::vector<VkBufferImageCopy> &regions) {
	return { srcBuffer, dstImage, dstImageLayout, regions };
}

struct copy_image_to_buffer {
	type::supplier<const image::image_type> srcImage;
	VkImageLayout srcImageLayout;
	type::supplier<const buffer::buffer_type> dstBuffer;
	std::vector<VkBufferImageCopy> regions;
};

struct update_buffer {
	type::supplier<const buffer::buffer_type> dstBuffer;
	VkDeviceSize dstOffset, dataSize;
	const uint32_t* pData;
};

struct fill_buffer {
	type::supplier<const buffer::buffer_type> dstBuffer;
	VkDeviceSize dstOffset, size;
	uint32_t data;
};

struct clear_color_image {
	type::supplier<const image::image_type> image;
	VkImageLayout imageLayout;
	VkClearColorValue color;
	std::vector<VkImageSubresourceRange> ranges;
};

struct clear_depth_stencil_image {
	type::supplier<const image::image_type> image;
	VkImageLayout imageLayout;
	VkClearDepthStencilValue pDepthStencil;
	std::vector<VkImageSubresourceRange> ranges;
};

struct clear_attachments {
	std::vector<VkClearAttachment> attachments;
	std::vector<VkClearRect> rects;
};

struct resolve_image {
	type::supplier<const image::image_type> srcImage;
	VkImageLayout srcImageLayout;
	type::supplier<const image::image_type> dstImage;
	VkImageLayout dstImageLayout;
	std::vector<VkImageResolve> regions;
};

struct set_event {
	type::supplier<const event::event_type> event;
	VkPipelineStageFlags stageMask;
};

struct reset_event {
	type::supplier<const event::event_type> event;
	VkPipelineStageFlags stageMask;
};

struct wait_events {
	std::vector<type::supplier<const event::event_type>> events;
	VkPipelineStageFlags srcStageMask, dstStageMask;
	std::vector<VkMemoryBarrier> memoryBarriers;
	std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers;
	std::vector<VkImageMemoryBarrier> imageMemoryBarriers;
};

struct memory_barrier {
	VkAccessFlags srcAccessMask, dstAccessMask;
};

struct buffer_memory_barrier_type {
	VkAccessFlags srcAccessMask, dstAccessMask;
	uint32_t srcQueueFamilyIndex, dstQueueFamilyIndex;
	type::supplier<const buffer::buffer_type> buffer;
	VkDeviceSize offset, size;
};

inline buffer_memory_barrier_type buffer_memory_barrier(
		VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
		uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex,
		const type::supplier<const buffer::buffer_type> &buffer,
		VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE) {
	return { srcAccessMask, dstAccessMask, srcQueueFamilyIndex, dstQueueFamilyIndex, buffer,
		offset, size };
}

inline buffer_memory_barrier_type buffer_memory_barrier(
		VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
		uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex,
		const type::supplier<const input_buffer::input_buffer_type> &buffer,
		VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE) {
	return { srcAccessMask, dstAccessMask, srcQueueFamilyIndex, dstQueueFamilyIndex,
		std::ref(input_buffer::internal::get_buffer(*buffer)), offset, size };
}

struct image_memory_barrier {
	VkAccessFlags srcAccessMask;
	VkAccessFlags dstAccessMask;
	VkImageLayout oldLayout;
	VkImageLayout newLayout;
	uint32_t srcQueueFamilyIndex;
	uint32_t dstQueueFamilyIndex;
	type::supplier<const image::image_type> image;
	VkImageSubresourceRange subresourceRange;
};

struct pipeline_barrier {
	pipeline_barrier(VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,
		const std::vector<memory_barrier> memory_barriers,
		const std::vector<buffer_memory_barrier_type> buffer_memory_barriers,
		const std::vector<image_memory_barrier> image_memory_barriers)
		: srcStageMask(srcStageMask), dstStageMask(dstStageMask),
		dependencyFlags(dependencyFlags), memory_barriers(memory_barriers),
		buffer_memory_barriers(buffer_memory_barriers),
		image_memory_barriers(image_memory_barriers) {}

	VkPipelineStageFlags srcStageMask, dstStageMask;
	VkDependencyFlags dependencyFlags;
	std::vector<memory_barrier> memory_barriers;
	std::vector<buffer_memory_barrier_type> buffer_memory_barriers;
	std::vector<image_memory_barrier> image_memory_barriers;
};

struct begin_query {
	type::supplier<const query_pool::query_pool_type> queryPool;
	uint32_t entry;
	VkQueryControlFlags flags;
};

struct end_query {
	type::supplier<const query_pool::query_pool_type> queryPool;
	uint32_t entry;
};

struct reset_query_pool {
	type::supplier<const query_pool::query_pool_type> queryPool;
	uint32_t firstQuery, queryCount;
};

struct write_timestamp {
	VkPipelineStageFlagBits pipelineStage;
	type::supplier<const query_pool::query_pool_type> queryPool;
	uint32_t entry;
};

struct copy_query_pool_results {
	type::supplier<const query_pool::query_pool_type> queryPool;
	uint32_t firstQuery, queryCount;
	type::supplier<const buffer::buffer_type> dstBuffer;
	VkDeviceSize dstOffset, stride;
	VkQueryResultFlags flags;
};

struct push_constants_type {
	type::supplier<const pipeline_layout::pipeline_layout_type> layout;
	VkShaderStageFlags stageFlags;
	uint32_t offset, size;
	const void* pValues;
};

inline push_constants_type push_constants(
		const type::supplier<const pipeline_layout::pipeline_layout_type> &layout,
		VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues) {
	return { layout, stageFlags, offset, size, pValues };
}

template<typename... CommandsT>
struct render_pass_type {
	type::supplier<const vcc::render_pass::render_pass_type> renderPass;
	type::supplier<const framebuffer::framebuffer_type> framebuffer;
	VkRect2D renderArea;
	std::vector<VkClearValue> clearValues;
	VkSubpassContents contents;
	std::tuple<CommandsT...> commands;
};

template<typename... CommandsT>
render_pass_type<CommandsT...> render_pass(
		const type::supplier<const render_pass::render_pass_type> &renderPass,
		const type::supplier<const framebuffer::framebuffer_type> &framebuffer,
		const VkRect2D &renderArea, const std::vector<VkClearValue> &clearValues,
		VkSubpassContents contents, CommandsT&&...  commands) {
	auto commands_tuple = std::make_tuple(std::forward<CommandsT>(commands)...);
	auto render_pass = render_pass_type<CommandsT...>{
		renderPass, framebuffer, renderArea, clearValues, contents,
		std::move(commands_tuple) };
	return std::move(render_pass);
}

struct next_subpass {
	VkSubpassContents contents;
};

struct execute_commands {
	std::vector<type::supplier<const command_buffer::command_buffer_type>> commandBuffers;
};

VCC_LIBRARY VkClearValue clear_color(const VkClearColorValue & color);

VCC_LIBRARY VkClearValue clear_depth_stencil(
	const VkClearDepthStencilValue &depth_stencil);

namespace internal {

VCC_LIBRARY void cmd(build_type &, const bind_pipeline &);
VCC_LIBRARY void cmd(build_type &, const set_viewport &);
VCC_LIBRARY void cmd(build_type &, const set_scissor &);
VCC_LIBRARY void cmd(build_type &, const set_line_width &);
VCC_LIBRARY void cmd(build_type &, const set_depth_bias &);
VCC_LIBRARY void cmd(build_type &, const set_blend_constants &);
VCC_LIBRARY void cmd(build_type &, const set_depth_bounds &);
VCC_LIBRARY void cmd(build_type &, const set_stencil_compare_mask &);
VCC_LIBRARY void cmd(build_type &, const set_stencil_write_mask &);
VCC_LIBRARY void cmd(build_type &, const set_stencil_reference &);
VCC_LIBRARY void cmd(build_type &, const bind_descriptor_sets &);
VCC_LIBRARY void cmd(build_type &, const bind_index_buffer_type &);
VCC_LIBRARY void cmd(build_type &, const bind_vertex_buffers_type &);
VCC_LIBRARY void cmd(build_type &, const draw &);
VCC_LIBRARY void cmd(build_type &, const draw_indexed &);
VCC_LIBRARY void cmd(build_type &, const draw_indirect_type &);
VCC_LIBRARY void cmd(build_type &, const draw_indexed_indirect_type &);
VCC_LIBRARY void cmd(build_type &, const dispatch &);
VCC_LIBRARY void cmd(build_type &, const dispatch_indirect_type &);
VCC_LIBRARY void cmd(build_type &, const copy_buffer_type &);
VCC_LIBRARY void cmd(build_type &, const copy_image &);
VCC_LIBRARY void cmd(build_type &, const blit_image &);
VCC_LIBRARY void cmd(build_type &, const copy_buffer_to_image_type &);
VCC_LIBRARY void cmd(build_type &, const copy_image_to_buffer &);
VCC_LIBRARY void cmd(build_type &, const update_buffer &);
VCC_LIBRARY void cmd(build_type &, const fill_buffer &);
VCC_LIBRARY void cmd(build_type &, const clear_color_image &);
VCC_LIBRARY void cmd(build_type &, const clear_depth_stencil_image &);
VCC_LIBRARY void cmd(build_type &, const clear_attachments &);
VCC_LIBRARY void cmd(build_type &, const resolve_image &);
VCC_LIBRARY void cmd(build_type &, const set_event &);
VCC_LIBRARY void cmd(build_type &, const reset_event &);
VCC_LIBRARY void cmd(build_type &, const wait_events &);
VCC_LIBRARY void cmd(build_type &, const pipeline_barrier &);
VCC_LIBRARY void cmd(build_type &, const begin_query &);
VCC_LIBRARY void cmd(build_type &, const end_query &);
VCC_LIBRARY void cmd(build_type &, const reset_query_pool &);
VCC_LIBRARY void cmd(build_type &, const write_timestamp &);
VCC_LIBRARY void cmd(build_type &, const copy_query_pool_results &);
VCC_LIBRARY void cmd(build_type &, const push_constants_type &);
VCC_LIBRARY void cmd(build_type &, const next_subpass &);
VCC_LIBRARY void cmd(build_type &, const execute_commands &);
VCC_LIBRARY void cmd(build_type &, const bind_index_data_buffer_type&);
VCC_LIBRARY void cmd(build_type &, const bind_vertex_data_buffers_type&);
VCC_LIBRARY void cmd(build_type &, const draw_indirect_data_type&);
VCC_LIBRARY void cmd(build_type &, const draw_indexed_indirect_data_type&);
VCC_LIBRARY void cmd(build_type &, const dispatch_indirect_data_type&);
VCC_LIBRARY void cmd(build_type &, const copy_data_buffer_type&);
VCC_LIBRARY void cmd(build_type &, const copy_data_buffer_to_image_type&);

// Need C++14 to do auto argument lambdas.
struct call_cmd_type {
	build_type &build;

	template<typename T>
	void operator()(T &value) {
		cmd(build, value);
	};
};

template<typename... CommandsT>
void cmd(build_type &build, render_pass_type<CommandsT...> &&render_pass) {
	VkRenderPassBeginInfo info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, NULL };
	info.renderPass = vcc::internal::get_instance(*render_pass.renderPass);
	info.framebuffer = vcc::internal::get_instance(*render_pass.framebuffer);
	info.renderArea = render_pass.renderArea;
	info.clearValueCount = (uint32_t)render_pass.clearValues.size();
	info.pClearValues = render_pass.clearValues.data();
	VKTRACE(vkCmdBeginRenderPass(vcc::internal::get_instance(get_command_buffer(build)), &info,
		render_pass.contents));
	util::tuple_foreach(call_cmd_type{ build }, render_pass.commands);
	VKTRACE(vkCmdEndRenderPass(vcc::internal::get_instance(get_command_buffer(build))));
	get_references(build).add(render_pass.renderPass, render_pass.framebuffer);
}

}  // namespace internal

template<typename... CommandsT>
void compile(build_type &begin, CommandsT&&... commands) {
	// int array guarantees order of execution with older GCC compilers.
	const int dummy[] = { (internal::cmd(begin, std::forward<CommandsT>(commands)), 0)... };
}

template<typename... CommandsT>
void compile(build_type &&begin, CommandsT&&... commands) {
	// int array guarantees order of execution with older GCC compilers.
	const int dummy[] = { (internal::cmd(begin, std::forward<CommandsT>(commands)), 0)... };
}

}  // namespace command
}  // namespace vcc

#endif  // _VCC_COMMAND_H_
