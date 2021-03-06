#ifndef SRC_VULKAN_COMMAND_BUFFER_HPP_
#define SRC_VULKAN_COMMAND_BUFFER_HPP_

#include "vulkan/Vulkan.hpp"

#include <memory>
#include <vector>

namespace scin {

namespace comp {
// TODO: this dependency back to comp namespace is a serious code smell.
class Scinth;
}

namespace vk {

class CommandPool;
class Device;

/*! Simple wrapper class around an array of Vulkan CommandBuffer objects.
 */
class CommandBuffer {
public:
    CommandBuffer(const std::shared_ptr<Device> device, std::shared_ptr<CommandPool> commandPool);
    ~CommandBuffer();

    /*! Allocate the command buffers internally.
     *
     * \param count The number of buffers to allocate.
     * \param isPrimary if true will allocate primary command buffers, if false will allocate secondary.
     * \return true on success, false on error.
     */
    bool create(size_t count, bool isPrimary);

    void destroy();

    /*! Associate a Scinth with the command buffer, so none of the associated resources in the Scinth will be destroyed
     * until after this CommandBuffer goes out of scope.
     */
    void associateScinth(std::shared_ptr<comp::Scinth> scinth);
    void associateSecondaryCommands(const std::vector<std::shared_ptr<CommandBuffer>>& secondary);

    VkCommandBuffer buffer(size_t i) { return m_commandBuffers[i]; }
    size_t count() const { return m_commandBuffers.size(); }

private:
    std::shared_ptr<Device> m_device;
    std::shared_ptr<CommandPool> m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::shared_ptr<comp::Scinth> m_scinth;
    std::vector<std::shared_ptr<CommandBuffer>> m_secondaryCommands;
};

} // namespace vk

} // namespace scin

#endif // SRC_VULKAN_COMMAND_BUFFER_HPP_
