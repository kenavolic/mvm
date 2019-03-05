// Copyright 2019 Ken Avolic <kenavolic@none.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

// @note Here the preprocessor is used to transform a string
//       into a char list. Gcc (-Wgnu-string-literal-operator-template)
//       and clang supports a way to achieve this through string literal
//       operator template
#define MVM_TO_STRING(x) #x
#define MVM_CHAR_LIST_1(x) MVM_TO_STRING(x)[0]
#define MVM_CHAR_LIST_2(x) MVM_CHAR_LIST_1(x), MVM_TO_STRING(x)[1]
#define MVM_CHAR_LIST_3(x) MVM_CHAR_LIST_2(x), MVM_TO_STRING(x)[2]
#define MVM_CHAR_LIST_4(x) MVM_CHAR_LIST_3(x), MVM_TO_STRING(x)[3]
#define MVM_CHAR_LIST_5(x) MVM_CHAR_LIST_4(x), MVM_TO_STRING(x)[4]
#define MVM_CHAR_LIST_6(x) MVM_CHAR_LIST_5(x), MVM_TO_STRING(x)[5]
#define MVM_CHAR_LIST_7(x) MVM_CHAR_LIST_6(x), MVM_TO_STRING(x)[6]
#define MVM_CHAR_LIST_8(x) MVM_CHAR_LIST_7(x), MVM_TO_STRING(x)[7]
#define MVM_CHAR_LIST_9(x) MVM_CHAR_LIST_8(x), MVM_TO_STRING(x)[8]
#define MVM_CHAR_LIST_10(x) MVM_CHAR_LIST_9(x), MVM_TO_STRING(x)[9]
#define MVM_CHAR_LIST_11(x) MVM_CHAR_LIST_10(x), MVM_TO_STRING(x)[10]
#define MVM_CHAR_LIST_12(x) MVM_CHAR_LIST_11(x), MVM_TO_STRING(x)[11]
#define MVM_CHAR_LIST_13(x) MVM_CHAR_LIST_12(x), MVM_TO_STRING(x)[12]
#define MVM_CHAR_LIST_14(x) MVM_CHAR_LIST_13(x), MVM_TO_STRING(x)[13]
#define MVM_CHAR_LIST_15(x) MVM_CHAR_LIST_14(x), MVM_TO_STRING(x)[14]
#define MVM_CHAR_LIST_16(x) MVM_CHAR_LIST_15(x), MVM_TO_STRING(x)[15]

#define MVM_CHAR_LIST(n, x) MVM_CHAR_LIST_##n(x)