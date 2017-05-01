/** @file */

#include "STM32IMUDevice.h"

/* Included for the std::vector<uint8_t> output function */
#include <YukariMSP/MSPClient.h>

using namespace Yukari::Common;
using namespace Yukari::Maths;

namespace Yukari
{
namespace IMU
{
  STM32IMUDevice::STM32IMUDevice(const std::string &port, unsigned int baud)
      : ISerialGrabber(port, baud)
      , m_logger(LoggingService::Instance().getLogger("STM32IMUDevice"))
  {
  }

  STM32IMUDevice::~STM32IMUDevice()
  {
  }

  IMUFrame::Ptr STM32IMUDevice::grabFrame()
  {
    if (!m_port.isOpen())
      return nullptr;

    size_t bytesTransfered = 0;
    size_t numBytesToRx;
    std::vector<uint8_t> buffer;

    /* Request data packet */
    bytesTransfered = m_port.write(std::vector<uint8_t>{'p'});

    numBytesToRx = 2;
    bytesTransfered = m_port.read(buffer, numBytesToRx);
    if (bytesTransfered != numBytesToRx)
    {
      m_logger->error("Incorrect number of bytes received: {} (expected {})", bytesTransfered,
                      numBytesToRx);
      return nullptr;
    }

    numBytesToRx = buffer[1] - 2;
    bytesTransfered = m_port.read(buffer, numBytesToRx);
    if (bytesTransfered != numBytesToRx)
    {
      m_logger->error("Incorrect number of bytes received: {} (expected {})", bytesTransfered,
                      numBytesToRx);
      return nullptr;
    }

    /* Parse data packet */
    union {
      ValuesPacket values;
      uint8_t data[sizeof(ValuesPacket)];
    } u;
    memcpy(u.data, buffer.data(), sizeof(ValuesPacket));

    /* Checksum */
    uint8_t rxChecksum = 0;
    for (size_t i = 0; i < 15; i++)
      rxChecksum ^= u.data[i];

    m_logger->trace("RX payload: {}, checksum: {}", buffer, rxChecksum);

    if (rxChecksum != u.values.checksum)
    {
      m_logger->warn("Checksum failed! ({} != {})", rxChecksum, u.values.checksum);
      return nullptr;
    }

    /* Create output frame */
    auto retVal = std::make_shared<IMUFrame>();

    retVal->orientation() =
        Eigen::Quaternionf((float)u.values.q_w / 10000.0f, -(float)u.values.q_x / 10000.0f,
                           (float)u.values.q_z / 10000.0f, (float)u.values.q_y / 10000.0f);

    retVal->position() =
        Eigen::Vector3f((float)u.values.d_x / 10000.0f, (float)u.values.d_y / 10000.0f,
                        (float)u.values.d_z / 10000.0f);

    return retVal;
  }
}
}
