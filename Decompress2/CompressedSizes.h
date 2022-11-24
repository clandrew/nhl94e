#pragma once

struct CompressedSizeEntry
{
    int TeamIndex;
    int PlayerIndex;
    int CompressedSize;
} s_compressedSizes[] = // These all decompress to 1152 bytes each.
{
 { 0, 0, 561},
 { 0, 1, 553},
 { 0, 2, 586},
 { 0, 3, 585},
 { 0, 4, 660},
 { 0, 5, 590},
 { 1, 0, 615},
 { 1, 1, 573},
 { 1, 2, 547},
 { 1, 3, 565},
 { 1, 4, 579},
 { 1, 5, 573},
 { 2, 0, 566},
 { 2, 1, 591},
 { 2, 2, 596},
 { 2, 3, 552},
 { 2, 4, 601},
 { 2, 5, 560},
 { 3, 0, 639},
 { 3, 1, 618},
 { 3, 2, 717},
 { 3, 3, 618},
 { 3, 4, 561},
 { 3, 5, 548},
 { 4, 0, 647},
 { 4, 1, 559},
 { 4, 2, 568},
 { 4, 3, 633},
 { 4, 4, 594},
 { 4, 5, 577},
 { 5, 0, 639},
 { 5, 1, 599},
 { 5, 2, 577},
 { 5, 3, 615},
 { 5, 4, 506},
 { 5, 5, 641},
 { 6, 0, 572},
 { 6, 1, 498},
 { 6, 2, 518},
 { 6, 3, 658},
 { 6, 4, 541},
 { 6, 5, 552},
 { 7, 0, 564},
 { 7, 1, 560},
 { 7, 2, 609},
 { 7, 3, 636},
 { 7, 4, 638},
 { 7, 5, 582},
 { 8, 0, 637},
 { 8, 1, 613},
 { 8, 2, 630},
 { 8, 3, 569},
 { 8, 4, 652},
 { 8, 5, 599},
 { 9, 0, 581},
 { 9, 1, 593},
 { 9, 2, 564},
 { 9, 3, 591},
 { 9, 4, 594},
 { 9, 5, 529},
 { 10, 0, 623},
 { 10, 1, 635},
 { 10, 2, 643},
 { 10, 3, 629},
 { 10, 4, 584},
 { 10, 5, 627},
 { 11, 0, 516},
 { 11, 1, 568},
 { 11, 2, 537},
 { 11, 3, 585},
 { 11, 4, 629},
 { 11, 5, 520},
 { 12, 0, 471},
 { 12, 1, 577},
 { 12, 2, 612},
 { 12, 3, 641},
 { 12, 4, 469},
 { 12, 5, 544},
 { 13, 0, 569},
 { 13, 1, 528},
 { 13, 2, 637},
 { 13, 3, 565},
 { 13, 4, 598},
 { 13, 5, 517},
 { 14, 0, 505},
 { 14, 1, 663},
 { 14, 2, 611},
 { 14, 3, 585},
 { 14, 4, 640},
 { 14, 5, 589},
 { 15, 0, 532},
 { 15, 1, 529},
 { 15, 2, 537},
 { 15, 3, 523},
 { 15, 4, 560},
 { 15, 5, 536},
 { 16, 0, 608},
 { 16, 1, 589},
 { 16, 2, 531},
 { 16, 3, 601},
 { 16, 4, 573},
 { 16, 5, 581},
 { 17, 0, 521},
 { 17, 1, 601},
 { 17, 2, 507},
 { 17, 3, 477},
 { 17, 4, 579},
 { 17, 5, 470},
 { 18, 0, 577},
 { 18, 1, 543},
 { 18, 2, 652},
 { 18, 3, 517},
 { 18, 4, 563},
 { 18, 5, 505},
 { 19, 0, 595},
 { 19, 1, 611},
 { 19, 2, 613},
 { 19, 3, 601},
 { 19, 4, 573},
 { 19, 5, 591},
 { 20, 0, 503},
 { 20, 1, 579},
 { 20, 2, 536},
 { 20, 3, 574},
 { 20, 4, 564},
 { 20, 5, 483},
 { 21, 0, 615},
 { 21, 1, 612},
 { 21, 2, 510},
 { 21, 3, 564},
 { 21, 4, 527},
 { 21, 5, 553},
 { 22, 0, 564},
 { 22, 1, 538},
 { 22, 2, 584},
 { 22, 3, 657},
 { 22, 4, 518},
 { 22, 5, 451},
 { 23, 0, 589},
 { 23, 1, 538},
 { 23, 2, 638},
 { 23, 3, 590},
 { 23, 4, 590},
 { 23, 5, 556},
 { 24, 0, 627},
 { 24, 1, 578},
 { 24, 2, 588},
 { 24, 3, 530},
 { 24, 4, 608},
 { 24, 5, 641},
 { 25, 0, 565},
 { 25, 1, 479},
 { 25, 2, 572},
 { 25, 3, 516},
 { 25, 4, 628},
 { 25, 5, 562},
 { 26, 0, 516},
 { 26, 1, 507},
 { 26, 2, 573},
 { 26, 3, 552},
 { 26, 4, 573},
 { 26, 5, 470},
 { 27, 0, 647},
 { 27, 1, 479},
 { 27, 2, 498},
 { 27, 3, 516},
 { 27, 4, 584},
 { 27, 5, 552},
};
