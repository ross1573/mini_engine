module mini.metal4;

import mini.core;
import mini.graphics;
import :log;
import :common;
import :buffer;
import :render_pass;

namespace mini::metal4 {

RenderPass::RenderPass(MTL::Device* device, MTL4::CommandBuffer* commandBuffer) noexcept
    : m_commandBuffer(nullptr)
    , m_renderCommandEncoder(nullptr)
    , m_targetTexture(nullptr)
{
    ASSERT(commandBuffer != nullptr);

    m_renderPassDescriptor = TransferShared(MTL4::RenderPassDescriptor::alloc());
    ENSURE(m_renderPassDescriptor.Valid(), "failed to create MTL4::RenderPassDescriptor") {
        return;
    }

    m_renderPassDescriptor->init();

    m_argumentTableDescriptor = TransferShared(MTL4::ArgumentTableDescriptor::alloc());
    ENSURE(m_argumentTableDescriptor.Valid(), "failed to create MTL4::ArgumentTableDescriptor") {
        return;
    }

    // TODO: get descriptor values from argument
    m_argumentTableDescriptor->init();
    m_argumentTableDescriptor->setMaxBufferBindCount(1);
    m_argumentTableDescriptor->setMaxSamplerStateBindCount(1);
    m_argumentTableDescriptor->setMaxTextureBindCount(1);
    m_argumentTableDescriptor->setSupportAttributeStrides(false);

    NS::Error* error = nullptr;
    MTL4::ArgumentTable* argumentTable = device->newArgumentTable(m_argumentTableDescriptor.Get(), &error);
    ENSURE(error == nullptr, "failed to create MTL4::ArgumentTable") {
        LogError(error->localizedFailureReason()->utf8String());
        return;
    }

    m_argumentTable = TransferShared(argumentTable);
    m_commandBuffer = commandBuffer;
}

RenderPass::~RenderPass() noexcept
{
    m_commandBuffer = nullptr;
    m_renderCommandEncoder = nullptr;
}

bool RenderPass::Valid() const noexcept
{
    return m_commandBuffer != nullptr;
}

bool RenderPass::Active() const noexcept
{
    return m_renderCommandEncoder != nullptr;
}

void RenderPass::Begin(MTL::Texture* targetTexture, Color const& color) noexcept
{
    m_targetTexture = targetTexture;

    ASSERT(m_commandBuffer != nullptr);
    ASSERT(m_targetTexture != nullptr);

    MTL::RenderPassColorAttachmentDescriptor* colorAttachment = m_renderPassDescriptor->colorAttachments()->object(0);
    MTL::ClearColor clearColor(static_cast<double>(color.r),
                               static_cast<double>(color.g),
                               static_cast<double>(color.b),
                               static_cast<double>(color.a));

    colorAttachment->setTexture(m_targetTexture);
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setStoreAction(MTL::StoreActionStore);
    colorAttachment->setClearColor(clearColor);
    colorAttachment->clearColor();

    MTL4::RenderPassDescriptor* renderPassDescriptor = m_renderPassDescriptor.Get();
    MTL4::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(renderPassDescriptor);

    ENSURE(renderCommandEncoder != nullptr, "failed to create MTL4::RenderCommandEncoder") {
        return;
    }

    m_renderCommandEncoder = renderCommandEncoder;
}

void RenderPass::End() noexcept
{
    ASSERT(m_renderCommandEncoder != nullptr);

    m_renderCommandEncoder->endEncoding();
    m_renderCommandEncoder = nullptr;
}

void RenderPass::SetVertexBuffer(Buffer const& buffer, uint64 index)
{
    ASSERT(m_renderCommandEncoder != nullptr);

    MTL::GPUAddress gpuAddress = static_cast<MTL::GPUAddress>(buffer.GpuAddress());
    m_argumentTable->setAddress(gpuAddress, index);
}

void RenderPass::DrawPrimitives(graphics::PrimitiveType primitiveType, uint64 vertexStart, uint64 vertexCount)
{
    ASSERT(m_renderCommandEncoder != nullptr);

    MTL::PrimitiveType mtlPrimitive = GetMTLPrimitiveType(primitiveType);
    MTL::RenderStages renderStages = MTL::RenderStageVertex;

    m_renderCommandEncoder->setArgumentTable(m_argumentTable.Get(), renderStages);
    m_renderCommandEncoder->drawPrimitives(mtlPrimitive, vertexStart, vertexCount);
}

void RenderPass::SetViewport(Rect const& rect, float32 near, float32 far) noexcept
{
    ASSERT(m_renderCommandEncoder != nullptr);

    MTL::Viewport viewport = {
        .originX = static_cast<double>(rect.x),
        .originY = static_cast<double>(rect.y),
        .width = static_cast<double>(rect.width),
        .height = static_cast<double>(rect.height),
        .znear = static_cast<double>(near),
        .zfar = static_cast<double>(far),
    };

    m_renderCommandEncoder->setViewport(viewport);
}

void RenderPass::SetScissorRect(RectInt const& rect) noexcept
{
    ASSERT(m_renderCommandEncoder != nullptr);

    MTL::ScissorRect ScissorRect = {
        .x = static_cast<NS::UInteger>(rect.x),
        .y = static_cast<NS::UInteger>(rect.y),
        .width = static_cast<NS::UInteger>(rect.width),
        .height = static_cast<NS::UInteger>(rect.height),
    };

    m_renderCommandEncoder->setScissorRect(ScissorRect);
}

} // namespace mini::metal4