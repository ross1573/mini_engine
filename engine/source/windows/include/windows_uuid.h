#ifndef WINDOWS_UUID_H
#define WINDOWS_UUID_H

#if PLATFORM_MINGW
#  define UUID(type)                               \
      template <>                                  \
      constexpr const GUID& __mingw_uuidof<type>() \
      {                                            \
          return IID_##type;                       \
      }
#else
#  define UUID(...)
#endif

#define UUID_1(type) UUID(type) UUID(type##1)
#define UUID_2(type) UUID_1(type) UUID(type##2)
#define UUID_3(type) UUID_2(type) UUID(type##3)
#define UUID_4(type) UUID_3(type) UUID(type##4)
#define UUID_5(type) UUID_4(type) UUID(type##5)
#define UUID_6(type) UUID_5(type) UUID(type##6)
#define UUID_7(type) UUID_6(type) UUID(type##7)
#define UUID_8(type) UUID_7(type) UUID(type##8)
#define UUID_9(type) UUID_8(type) UUID(type##9)

#endif // WINDOWS_UUID_H