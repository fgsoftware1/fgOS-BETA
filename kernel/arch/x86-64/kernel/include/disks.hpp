#ifndef DISKS_H
#define DISKS_H

#include "datetime.hpp"
#include "stl/types.hpp"
#include "stl/array.hpp"
#include "stl/vector.hpp"
#include "stl/string.hpp"

namespace disks
{
  enum class disk_type
  {
    ATA
  };

  struct disk_descriptor
  {
    uint64_t uuid;
    disk_type type;
    void *descriptor;
  };

  enum class partition_type
  {
    FAT32,
    UNKNOWN
  };

  struct partition_descriptor
  {
    uint64_t uuid;
    partition_type type;
    uint64_t start;
    uint64_t sectors;
  };

  struct file
  {
    std::string file_name;
    bool directory;
    bool hidden;
    bool system;
    uint64_t size;
    datetime created;
    datetime modified;
    datetime accessed;

    size_t location;
    size_t position;
  };

  void detect_disks();

  uint64_t detected_disks();

  bool disk_exists(uint64_t uuid);

  const disk_descriptor &disk_by_index(uint64_t index);
  const disk_descriptor &disk_by_uuid(uint64_t uuid);

  const char *disk_type_to_string(disk_type type);
  const char *partition_type_to_string(partition_type type);

  bool read_sectors(const disk_descriptor &disk, uint64_t start, uint8_t count, void *destination);
  bool write_sectors(const disk_descriptor &disk, uint64_t start, uint8_t count, void *destination);
  std::unique_heap_array<partition_descriptor> partitions(const disk_descriptor &disk);
  bool partition_exists(const disk_descriptor &disk, uint64_t uuid);

  void mount(const disk_descriptor &disk, uint64_t uuid);
  void unmount();
  std::vector<file> ls();
  uint64_t free_size();

  bool mkdir(const std::string &directory);
  bool touch(const std::string &file);
  bool rm(const std::string &file);
  std::string read_file(const std::string &file);

  const disk_descriptor *mounted_disk();
  const partition_descriptor *mounted_partition();

  std::vector<std::string> &current_directory();

}

#endif
