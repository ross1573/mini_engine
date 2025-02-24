#pragma once

#define CONCAT_INNER(x, y) x ## y
#define CONCAT(x, y) CONCAT_INNER(x, y)

#define DELETE(x) if (x) { delete x; x = nullptr; }