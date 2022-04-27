#ifndef ATA_HPP
#define ATA_HPP

#include "../stl/types.hpp"
#include "../stl/string.hpp"

namespace ata
{
  struct drive_descriptor
  {
    uint16_t controller;
    uint8_t drive;
    bool present;
    uint8_t slave;
    std::string model;
    std::string serial;
    std::string firmware;
  };

  void detect_disks();
  uint8_t number_of_disks();
  drive_descriptor &drive(uint8_t disk);

  bool read_sectors(drive_descriptor &drive, uint64_t start, uint8_t count, void *destination);
  bool write_sectors(drive_descriptor &drive, uint64_t start, uint8_t count, void *source);
}

#endif
