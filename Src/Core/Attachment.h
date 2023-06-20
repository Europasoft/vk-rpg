#pragma once
#include "Core/vk.h"
#include "Core/GPU/Image.h"
#include <memory>
#include <vector>

namespace EngineCore
{
	class EngineSwapChain;
	class EngineDevice;
	
	enum class AttachmentType { COLOR, RESOLVE, DEPTH, DEPTH_STENCIL };

	struct AttachmentProperties 
	{
		AttachmentType type; // image creation
		VkExtent2D extent; // image creation
		VkFormat format; // image creation
		uint32_t imageCount; // image creation
		VkSampleCountFlagBits samples; // image creation

		AttachmentProperties(AttachmentType type) : type{ type }, extent{}, format{}, imageCount{}, samples{} {};
		VkImageAspectFlags getAspectFlags() const;
	};
	
	// handles the image resources for a framebuffer attachment, may be used in multiple framebuffers
	class Attachment
	{
	public:
		Attachment(EngineDevice& device, const AttachmentProperties& props, bool input, bool sampled);
		// swapchain attachment constructor
		Attachment(EngineDevice& device, const AttachmentProperties& props, const std::vector<VkImage>& swapchainImages);
		Attachment(Attachment&&) = default;

		std::vector<VkImageView> getImageViews() const;
		const AttachmentProperties& getProps() const { return props; }
		bool isCompatible(const Attachment& b) const;
		static bool isColor(AttachmentType t) { return t == AttachmentType::COLOR || t == AttachmentType::RESOLVE; }

	private:
		class EngineDevice& device;
		std::vector<std::unique_ptr<Image>> images;
		AttachmentProperties props;
	};


	// just syntactic sugar, e.g. AttachmentLoadOp::LOAD
	struct AttachmentLoadOp
	{
		const static VkAttachmentLoadOp DONT_CARE = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		const static VkAttachmentLoadOp LOAD = VK_ATTACHMENT_LOAD_OP_LOAD;
		const static VkAttachmentLoadOp CLEAR = VK_ATTACHMENT_LOAD_OP_CLEAR;
	};
	struct AttachmentStoreOp
	{
		const static VkAttachmentStoreOp DONT_CARE = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		const static VkAttachmentStoreOp STORE = VK_ATTACHMENT_STORE_OP_STORE;
	};
	// attachment info for renderpass and framebuffer creation
	struct AttachmentUse
	{
		std::vector<VkImageView> imageViews;
		VkAttachmentDescription description{};
		AttachmentType type;

		AttachmentUse(const Attachment& attachment, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
						VkImageLayout initialLayout, VkImageLayout finalLayout);
		void setStencilOps(VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp);
	};

}