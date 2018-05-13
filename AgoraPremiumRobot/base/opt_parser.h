#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

namespace agora {
namespace base {
struct ipv4 {
  union {
    uint32_t ip;
    uint8_t repr[4];
  };
};

struct mac_addr {
  uint8_t addr_bytes[6];
};

class opt_parser {
  enum pointer_type {kBool, kInt32, kUInt32, kInt64, kUInt64, kDouble, kString,
      kIPv4, kMacAddr};
  struct internal_opt {
    pointer_type type;
    union {
      bool *bool_ptr;
      int32_t *int32_ptr;
      uint32_t *uint32_ptr;
      int64_t *int64_ptr;
      uint64_t *uint64_ptr;
      double *double_ptr;
      std::string *string_ptr;
      ipv4 *ipv4_ptr;
      mac_addr *addr_ptr;
    };
  };
 public:
  opt_parser() {}

  // bool add_short_arg(bool *store, char short_arg);
  // bool add_short_arg(int *store, char short_arg);
  // bool add_short_arg(std::string *store, char short_arg);

  bool add_long_opt(const char *long_opt, bool *store);
  bool add_long_opt(const char *long_opt, int32_t *store);
  bool add_long_opt(const char *long_opt, uint32_t *store);
  bool add_long_opt(const char *long_opt, int64_t *store);
  bool add_long_opt(const char *long_opt, uint64_t *store);
  bool add_long_opt(const char *long_opt, double *store);
  bool add_long_opt(const char *long_opt, std::string *store);
  bool add_long_opt(const char *long_opt, ipv4 *store);
  bool add_long_opt(const char *long_opt, mac_addr *store);

  // NOTE(liuyong): the FIRST argument must be supplied as a place holder
  bool parse_opts(int argc, char *const argv[]);

  void clear();

  void print_usage(const char *exec_file, std::ostream &sout) const;
 private:
  // bool insert_short_arg(internal_opt arg, char short_arg);
  bool insert_long_opt(internal_opt opt, const char *long_opt);

  static bool fill_arg(const char *opt_name, const internal_opt &opt,
      const char *opt_arg);

  static bool parse_ipv4(const char *arg, ipv4 *ip);
  static bool parse_mac_addr(const char *arg, mac_addr *addr);
 private:
  // std::unordered_map<char, internal_opt> short_args_;
  std::unordered_map<const char *, internal_opt> long_opts_;
};

}
}

