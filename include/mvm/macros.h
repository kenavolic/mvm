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

#define MVM_UNROLL_256(MACRO_TO_UNROLL)                                        \
  MACRO_TO_UNROLL(0)                                                           \
  MACRO_TO_UNROLL(1)                                                           \
  MACRO_TO_UNROLL(2)                                                           \
  MACRO_TO_UNROLL(3)                                                           \
  MACRO_TO_UNROLL(4)                                                           \
  MACRO_TO_UNROLL(5)                                                           \
  MACRO_TO_UNROLL(6)                                                           \
  MACRO_TO_UNROLL(7)                                                           \
  MACRO_TO_UNROLL(8)                                                           \
  MACRO_TO_UNROLL(9)                                                           \
  MACRO_TO_UNROLL(10)                                                          \
  MACRO_TO_UNROLL(11)                                                          \
  MACRO_TO_UNROLL(12)                                                          \
  MACRO_TO_UNROLL(13)                                                          \
  MACRO_TO_UNROLL(14)                                                          \
  MACRO_TO_UNROLL(15)                                                          \
  MACRO_TO_UNROLL(16)                                                          \
  MACRO_TO_UNROLL(17)                                                          \
  MACRO_TO_UNROLL(18)                                                          \
  MACRO_TO_UNROLL(19)                                                          \
  MACRO_TO_UNROLL(20)                                                          \
  MACRO_TO_UNROLL(21)                                                          \
  MACRO_TO_UNROLL(22)                                                          \
  MACRO_TO_UNROLL(23)                                                          \
  MACRO_TO_UNROLL(24)                                                          \
  MACRO_TO_UNROLL(25)                                                          \
  MACRO_TO_UNROLL(26)                                                          \
  MACRO_TO_UNROLL(27)                                                          \
  MACRO_TO_UNROLL(28)                                                          \
  MACRO_TO_UNROLL(29)                                                          \
  MACRO_TO_UNROLL(30)                                                          \
  MACRO_TO_UNROLL(31)                                                          \
  MACRO_TO_UNROLL(32)                                                          \
  MACRO_TO_UNROLL(33)                                                          \
  MACRO_TO_UNROLL(34)                                                          \
  MACRO_TO_UNROLL(35)                                                          \
  MACRO_TO_UNROLL(36)                                                          \
  MACRO_TO_UNROLL(37)                                                          \
  MACRO_TO_UNROLL(38)                                                          \
  MACRO_TO_UNROLL(39)                                                          \
  MACRO_TO_UNROLL(40)                                                          \
  MACRO_TO_UNROLL(41)                                                          \
  MACRO_TO_UNROLL(42)                                                          \
  MACRO_TO_UNROLL(43)                                                          \
  MACRO_TO_UNROLL(44)                                                          \
  MACRO_TO_UNROLL(45)                                                          \
  MACRO_TO_UNROLL(46)                                                          \
  MACRO_TO_UNROLL(47)                                                          \
  MACRO_TO_UNROLL(48)                                                          \
  MACRO_TO_UNROLL(49)                                                          \
  MACRO_TO_UNROLL(50)                                                          \
  MACRO_TO_UNROLL(51)                                                          \
  MACRO_TO_UNROLL(52)                                                          \
  MACRO_TO_UNROLL(53)                                                          \
  MACRO_TO_UNROLL(54)                                                          \
  MACRO_TO_UNROLL(55)                                                          \
  MACRO_TO_UNROLL(56)                                                          \
  MACRO_TO_UNROLL(57)                                                          \
  MACRO_TO_UNROLL(58)                                                          \
  MACRO_TO_UNROLL(59)                                                          \
  MACRO_TO_UNROLL(60)                                                          \
  MACRO_TO_UNROLL(61)                                                          \
  MACRO_TO_UNROLL(62)                                                          \
  MACRO_TO_UNROLL(63)                                                          \
  MACRO_TO_UNROLL(64)                                                          \
  MACRO_TO_UNROLL(65)                                                          \
  MACRO_TO_UNROLL(66)                                                          \
  MACRO_TO_UNROLL(67)                                                          \
  MACRO_TO_UNROLL(68)                                                          \
  MACRO_TO_UNROLL(69)                                                          \
  MACRO_TO_UNROLL(70)                                                          \
  MACRO_TO_UNROLL(71)                                                          \
  MACRO_TO_UNROLL(72)                                                          \
  MACRO_TO_UNROLL(73)                                                          \
  MACRO_TO_UNROLL(74)                                                          \
  MACRO_TO_UNROLL(75)                                                          \
  MACRO_TO_UNROLL(76)                                                          \
  MACRO_TO_UNROLL(77)                                                          \
  MACRO_TO_UNROLL(78)                                                          \
  MACRO_TO_UNROLL(79)                                                          \
  MACRO_TO_UNROLL(80)                                                          \
  MACRO_TO_UNROLL(81)                                                          \
  MACRO_TO_UNROLL(82)                                                          \
  MACRO_TO_UNROLL(83)                                                          \
  MACRO_TO_UNROLL(84)                                                          \
  MACRO_TO_UNROLL(85)                                                          \
  MACRO_TO_UNROLL(86)                                                          \
  MACRO_TO_UNROLL(87)                                                          \
  MACRO_TO_UNROLL(88)                                                          \
  MACRO_TO_UNROLL(89)                                                          \
  MACRO_TO_UNROLL(90)                                                          \
  MACRO_TO_UNROLL(91)                                                          \
  MACRO_TO_UNROLL(92)                                                          \
  MACRO_TO_UNROLL(93)                                                          \
  MACRO_TO_UNROLL(94)                                                          \
  MACRO_TO_UNROLL(95)                                                          \
  MACRO_TO_UNROLL(96)                                                          \
  MACRO_TO_UNROLL(97)                                                          \
  MACRO_TO_UNROLL(98)                                                          \
  MACRO_TO_UNROLL(99)                                                          \
  MACRO_TO_UNROLL(100)                                                         \
  MACRO_TO_UNROLL(101)                                                         \
  MACRO_TO_UNROLL(102)                                                         \
  MACRO_TO_UNROLL(103)                                                         \
  MACRO_TO_UNROLL(104)                                                         \
  MACRO_TO_UNROLL(105)                                                         \
  MACRO_TO_UNROLL(106)                                                         \
  MACRO_TO_UNROLL(107)                                                         \
  MACRO_TO_UNROLL(108)                                                         \
  MACRO_TO_UNROLL(109)                                                         \
  MACRO_TO_UNROLL(110)                                                         \
  MACRO_TO_UNROLL(111)                                                         \
  MACRO_TO_UNROLL(112)                                                         \
  MACRO_TO_UNROLL(113)                                                         \
  MACRO_TO_UNROLL(114)                                                         \
  MACRO_TO_UNROLL(115)                                                         \
  MACRO_TO_UNROLL(116)                                                         \
  MACRO_TO_UNROLL(117)                                                         \
  MACRO_TO_UNROLL(118)                                                         \
  MACRO_TO_UNROLL(119)                                                         \
  MACRO_TO_UNROLL(120)                                                         \
  MACRO_TO_UNROLL(121)                                                         \
  MACRO_TO_UNROLL(122)                                                         \
  MACRO_TO_UNROLL(123)                                                         \
  MACRO_TO_UNROLL(124)                                                         \
  MACRO_TO_UNROLL(125)                                                         \
  MACRO_TO_UNROLL(126)                                                         \
  MACRO_TO_UNROLL(127)                                                         \
  MACRO_TO_UNROLL(128)                                                         \
  MACRO_TO_UNROLL(129)                                                         \
  MACRO_TO_UNROLL(130)                                                         \
  MACRO_TO_UNROLL(131)                                                         \
  MACRO_TO_UNROLL(132)                                                         \
  MACRO_TO_UNROLL(133)                                                         \
  MACRO_TO_UNROLL(134)                                                         \
  MACRO_TO_UNROLL(135)                                                         \
  MACRO_TO_UNROLL(136)                                                         \
  MACRO_TO_UNROLL(137)                                                         \
  MACRO_TO_UNROLL(138)                                                         \
  MACRO_TO_UNROLL(139)                                                         \
  MACRO_TO_UNROLL(140)                                                         \
  MACRO_TO_UNROLL(141)                                                         \
  MACRO_TO_UNROLL(142)                                                         \
  MACRO_TO_UNROLL(143)                                                         \
  MACRO_TO_UNROLL(144)                                                         \
  MACRO_TO_UNROLL(145)                                                         \
  MACRO_TO_UNROLL(146)                                                         \
  MACRO_TO_UNROLL(147)                                                         \
  MACRO_TO_UNROLL(148)                                                         \
  MACRO_TO_UNROLL(149)                                                         \
  MACRO_TO_UNROLL(150)                                                         \
  MACRO_TO_UNROLL(151)                                                         \
  MACRO_TO_UNROLL(152)                                                         \
  MACRO_TO_UNROLL(153)                                                         \
  MACRO_TO_UNROLL(154)                                                         \
  MACRO_TO_UNROLL(155)                                                         \
  MACRO_TO_UNROLL(156)                                                         \
  MACRO_TO_UNROLL(157)                                                         \
  MACRO_TO_UNROLL(158)                                                         \
  MACRO_TO_UNROLL(159)                                                         \
  MACRO_TO_UNROLL(160)                                                         \
  MACRO_TO_UNROLL(161)                                                         \
  MACRO_TO_UNROLL(162)                                                         \
  MACRO_TO_UNROLL(163)                                                         \
  MACRO_TO_UNROLL(164)                                                         \
  MACRO_TO_UNROLL(165)                                                         \
  MACRO_TO_UNROLL(166)                                                         \
  MACRO_TO_UNROLL(167)                                                         \
  MACRO_TO_UNROLL(168)                                                         \
  MACRO_TO_UNROLL(169)                                                         \
  MACRO_TO_UNROLL(170)                                                         \
  MACRO_TO_UNROLL(171)                                                         \
  MACRO_TO_UNROLL(172)                                                         \
  MACRO_TO_UNROLL(173)                                                         \
  MACRO_TO_UNROLL(174)                                                         \
  MACRO_TO_UNROLL(175)                                                         \
  MACRO_TO_UNROLL(176)                                                         \
  MACRO_TO_UNROLL(177)                                                         \
  MACRO_TO_UNROLL(178)                                                         \
  MACRO_TO_UNROLL(179)                                                         \
  MACRO_TO_UNROLL(180)                                                         \
  MACRO_TO_UNROLL(181)                                                         \
  MACRO_TO_UNROLL(182)                                                         \
  MACRO_TO_UNROLL(183)                                                         \
  MACRO_TO_UNROLL(184)                                                         \
  MACRO_TO_UNROLL(185)                                                         \
  MACRO_TO_UNROLL(186)                                                         \
  MACRO_TO_UNROLL(187)                                                         \
  MACRO_TO_UNROLL(188)                                                         \
  MACRO_TO_UNROLL(189)                                                         \
  MACRO_TO_UNROLL(190)                                                         \
  MACRO_TO_UNROLL(191)                                                         \
  MACRO_TO_UNROLL(192)                                                         \
  MACRO_TO_UNROLL(193)                                                         \
  MACRO_TO_UNROLL(194)                                                         \
  MACRO_TO_UNROLL(195)                                                         \
  MACRO_TO_UNROLL(196)                                                         \
  MACRO_TO_UNROLL(197)                                                         \
  MACRO_TO_UNROLL(198)                                                         \
  MACRO_TO_UNROLL(199)                                                         \
  MACRO_TO_UNROLL(200)                                                         \
  MACRO_TO_UNROLL(201)                                                         \
  MACRO_TO_UNROLL(202)                                                         \
  MACRO_TO_UNROLL(203)                                                         \
  MACRO_TO_UNROLL(204)                                                         \
  MACRO_TO_UNROLL(205)                                                         \
  MACRO_TO_UNROLL(206)                                                         \
  MACRO_TO_UNROLL(207)                                                         \
  MACRO_TO_UNROLL(208)                                                         \
  MACRO_TO_UNROLL(209)                                                         \
  MACRO_TO_UNROLL(210)                                                         \
  MACRO_TO_UNROLL(211)                                                         \
  MACRO_TO_UNROLL(212)                                                         \
  MACRO_TO_UNROLL(213)                                                         \
  MACRO_TO_UNROLL(214)                                                         \
  MACRO_TO_UNROLL(215)                                                         \
  MACRO_TO_UNROLL(216)                                                         \
  MACRO_TO_UNROLL(217)                                                         \
  MACRO_TO_UNROLL(218)                                                         \
  MACRO_TO_UNROLL(219)                                                         \
  MACRO_TO_UNROLL(220)                                                         \
  MACRO_TO_UNROLL(221)                                                         \
  MACRO_TO_UNROLL(222)                                                         \
  MACRO_TO_UNROLL(223)                                                         \
  MACRO_TO_UNROLL(224)                                                         \
  MACRO_TO_UNROLL(225)                                                         \
  MACRO_TO_UNROLL(226)                                                         \
  MACRO_TO_UNROLL(227)                                                         \
  MACRO_TO_UNROLL(228)                                                         \
  MACRO_TO_UNROLL(229)                                                         \
  MACRO_TO_UNROLL(230)                                                         \
  MACRO_TO_UNROLL(231)                                                         \
  MACRO_TO_UNROLL(232)                                                         \
  MACRO_TO_UNROLL(233)                                                         \
  MACRO_TO_UNROLL(234)                                                         \
  MACRO_TO_UNROLL(235)                                                         \
  MACRO_TO_UNROLL(236)                                                         \
  MACRO_TO_UNROLL(237)                                                         \
  MACRO_TO_UNROLL(238)                                                         \
  MACRO_TO_UNROLL(239)                                                         \
  MACRO_TO_UNROLL(240)                                                         \
  MACRO_TO_UNROLL(241)                                                         \
  MACRO_TO_UNROLL(242)                                                         \
  MACRO_TO_UNROLL(243)                                                         \
  MACRO_TO_UNROLL(244)                                                         \
  MACRO_TO_UNROLL(245)                                                         \
  MACRO_TO_UNROLL(246)                                                         \
  MACRO_TO_UNROLL(247)                                                         \
  MACRO_TO_UNROLL(248)                                                         \
  MACRO_TO_UNROLL(249)                                                         \
  MACRO_TO_UNROLL(250)                                                         \
  MACRO_TO_UNROLL(251)                                                         \
  MACRO_TO_UNROLL(252)                                                         \
  MACRO_TO_UNROLL(253)                                                         \
  MACRO_TO_UNROLL(254)                                                         \
  MACRO_TO_UNROLL(255)
