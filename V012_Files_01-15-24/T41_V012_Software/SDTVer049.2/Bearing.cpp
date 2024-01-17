#ifndef BEENHERE
#include "SDT.h"
#endif
/*
  #define SD_CS                       BUILTIN_SDCARD        // Works on T_3.6 and T_4.1 ...

  #define MY_LON                      -84.42676671875002
  #define MY_LAT                      39.074660204008886
  #define CENTER_SCREEN_X             387
  #define CENTER_SCREEN_Y             240
  #define IMAGE_CORNER_X              211 // ImageWidth = 378 Therefore 800 - 378 = 422 / 2 = 211
  #define IMAGE_CORNER_Y              89  // ImageHeight = 302 Therefore 480 - 302 = 178 / 2 = 89
  #define RAY_LENGTH                  190
  #define BUFFPIXEL                   20  // Use buffer to read image rather than 1 pixel at a time

  #define DEGREES2RADIANS             0.01745329
  #define RADIANS2DEGREES             57.29578
  #define PI_BY_180                   0.01745329
*/
enum { SCL_HALF = 0,
       SCL_QUARTER,
       SCL_EIGHTH,
       SCL_16TH
};
#define MAX_KEY_NAME 20
typedef struct {
  const char key_name[MAX_KEY_NAME];
  int *key_value_addr;
} key_name_value_t;


uint8_t sdbuffer[3 * BUFFPIXEL];  // pixel buffer (R+G+B per pixel)


int g_background_color = RA8875_BLACK;
int g_debug_output = 0;
int g_jpg_scale_x_above[4];
int g_jpg_scale_y_above[4];
int g_stepMode;
int g_BMPScale;
int g_JPGScale;
int g_PNGScale;
int g_center_image;
int g_display_image_time = 2500;
int g_max_scale_up = 4;
int g_image_width;
int g_image_height;
int g_image_offset_x;
int g_image_offset_y;
uint8_t g_image_scale;
uint8_t g_image_scale_up;
int g_tft_height;
int g_tft_width;
int g_WRCount;
// debug count how many time writeRect called
int keyCell;  // Where to place the cell in the X axis
//const int chipSelect          = BUILTIN_SDCARD;
uint8_t g_ra8875_layer_active = 0;

float bearingDegrees;
float bearingRadians;
float bearingDistance;
float displayBearing;  // Different because of image distortion


#ifdef TFT_CLIP_SUPPORT
inline void writeClippedRect(int16_t x, int16_t y, int16_t cx, int16_t cy, uint16_t *pixels, bool waitForWRC = true) {
  tft.writeRect(x + g_image_offset_x, y + g_image_offset_y, cx, cy, pixels);
}
#else
void writeClippedRect(int x, int y, int cx, int cy, uint16_t *pixels, bool waitForWRC = true);
#endif

char letters[] = { '\0',  // Make a 1's-based array
                   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
                   'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                   'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '*',  // Space is for SP
                   '0', '1', '2', '3', '4', '5', '6', '7', '8',
                   '9' };

int spacing[] = { 0, 70, 140, 210, 280, 350, 420, 490, 560, 630 };

struct cities {
  char callPrefix[12];
  char country[30];
  double lat;
  double lon;
};

//float homeLat = MY_LAT;  // your QTH latitude
//float homeLon = MY_LON;  // your QTH longitude
float homeLat = myMapFiles[0].lat;
float homeLon = myMapFiles[0].lon;  // your QTH longitude
float dxLat;
float dxLon;

PROGMEM struct cities dxCities[] = {
  // From John G0ORX.  Increases stack size.
  // callPrefix country  lat lon
  "1A", "MALTA ORDER", 45, 12.5,
  "3A", "MONACO", 43.75, 7.5,
  "3B6", "AGALEGA Is.", -10.5, 56.5,
  "3B7", "ST BRANDON Is.", -10.5, 56.5,
  "3B8", "MAURITIUS", -20, 57.5,
  "3B9", "RODRIGUEZ Is.", -20, 63.5,
  "3C", "EQUATORIAL GUINEA", 4, 9,
  "3C0", "ANNOBON Is.", -1.5, 5.5,
  "3D2", "FIJI Is.", -17.6, 178,
  "3D2C", "CONWAY REEF", -21.8, 174.5,
  "3D2R", "ROTUMA Is.", -12.5, 177.8,
  "3D6", "SWAZILAND", -27, 32,
  "3DA", "KINGDOM OF ESWATINI", -26.305, 31.146,
  "3V", "TUNISIA", 37, 10,
  "3W", "VIET NAM", 14.645, 108.336,
  "XV", "VIET NAM", 14.645, 108.336,
  "3Y5", "BOUVET Is.", -54.5, 3.5,
  "3Y0", "PETER Is.", -68.81, -90.5,
  "4K", "AZERBAIDJAN", 40.5, 50,
  "4L", "GEORGIA", 42, 45,
  "4O", "MONTENEGRO", 42.403, 19.256,
  "4S", "SRI LANKA", 7, 81,
  "4U1ITU", "GENEVA ITU", 46.25, 6.15,
  "4U1UN", "UN HQ NEW YORK", 41, -74,
  "4W", "TIMOR - LESTE", -8.868, 125.501,
  "4X", "ISRAEL", 32, 35,
  "5A", "LIBYA", 32.5, 12.5,
  "5B", "CYPRUS", 35, 33,
  "5H", "TANZANIA", -7, 39,
  "5N", "NIGERIA", 6.5, 3,
  "5R", "MALAGASY REP", -18.5, 47,
  "5T", "MAURITANIA", 18, -16,
  "5U", "NIGER REP", 13.5, 2,
  "5V", "TOGO REP", 6, 1.5,
  "5W", "WESTERN SAMOA", -13, -172,
  "5X", "UGANDA", 1, 32.5,
  "5y", "KENYA", -1.5, 37.5,
  "5Z", "KENYA", -1.5, 37.5,
  "6O", "SOMALI", 2, 46,
  "6W", "SENEGAL", 15, -18,
  "6Y", "JAMAICA", 18, -77,
  "7J", "JAPAN", 35.7, 139.7,
  "7O", "YEMEN", 13, 45,
  "7P", "LESOTHO", -30, 28,
  "7Q", "MALAWI", -15, 35,
  "7X", "ALGERIA", 37, 3,
  "8P", "BARBADOS", 13, -60,
  "8Q", "MALDIVE Is.", 4.5, 73.5,
  "8R", "GUYANA", 7, -58,
  "9A", "CROATIA", 46, 16,
  "9G", "GHANA", 5.5, 0,
  "9H", "MALTA", 36, 14.5,
  "9J", "ZAMBIA", -15, 28,
  "9K", "KUWAIT", 29, 48,
  "9L", "SIERRA LEONE", 8.5, -13,
  "9M2", "W MALAYSIA", 3, 102,
  "9M6", "E MALAYSIA", 6, 117,
  "9N", "NEPAL", 27.5, 85,
  "9Q", "DEM REP. OF CONGO", -4, 15,
  "9U", "BURUNDI", -3, 29,
  "9V", "SINGAPORE", 1, 104,
  "9X", "RWANDA", -2, 30,
  "9Y", "TRINIDAD-TOBAGO", 10.5, -61.5,
  "A2", "BOTSWANA", -24.662, 25.986,
  "A3", "TONGA Is.", -21.196, -175.186,
  "A4", "OMAN", 22, 58,
  "A5", "BHUTAN", 27.5, 90.1,
  "A6", "UNITED ARAB EMIRATES", 24.432, 54.396,
  "A7", "QATAR", 25, 51.2,
  "A9", "BAHRAIN", 26, 50.5,
  "AP2", "PAKISTAN", 30, 70,
  "AS", "SPRATLY Is.", 8.645, 111.920,
  "B", "CHINA", 39.863, 116.425,
  "BS7", "SCARBOROUGH", 15, 118,
  "BV", "TAIWAN", 24, 121,
  "BV9P", "PRATAS Is.", 21, 117,
  "BY", "CHINA", 39.863, 116.425,
  "C2", "NAURU", -.5, 167,
  "C3", "ANDORRA", 42.5, 1.5,
  "C5", "THE GAMBIA", 13, -17,
  "C6", "BAHAMAS", 25, -77.5,
  "C9", "MOZAMBIQUE", -20, 33,
  "CE", "CHILE", -33.5, -71.5,
  "CE0A", "EASTER Is.", -28, -109,
  "CE0X", "SAN FELIX", -26, -80,
  "CE0Z", "JUAN FERNANDEZ", -33.5, -78.7,
  "CE9", "ANTARCTICA", -74.555, -0.63,
  "CM", "CUBA", 23, -82,
  "CN", "MOROCCO", 33, -8,
  "CO", "CUBA", 23, -82,
  "CP", "BOLIVIA", -16.5, -68.5,
  "CT", "PORTUGAL", 39, -9,
  "CT3", "MADEIRA Is.", 33, -17,
  "CU", "AZORES Is.", 38, -28,
  "CV", "URUGUAY", -35, -56,
  "CX", "URUGUAY", -35, -56,
  "CY0", "SABLE Is.", 43.93, -60.00,
  "CY9", "ST PAUL Is.", 47.25, -60.15,
  "D2", "ANGOLA", -8.7, 13.15,
  "D4", "CAPE VERDE Is.", 16, -24,
  "D6", "COMOROS Is.", -12, 44,
  "DL", "GERMANY", 52.526, 13.637,
  "DU", "PHILIPPINES", 15, 121,
  "E3", "ERITHREA", 15, 39,
  "E4", "PALESTINE", 31.999, 35.224,
  "E5", "COOK IS.", -21.228, -159.785,
  "E6", "NIUE", -19.06, -169.8634,
  "E7", "BOSNIA-HERZEGOVINA", 43.867, 18.412,
  "EA", "SPAIN", 40, -4,
  "EA3", "CATALALONIA", 42.381, 1.167,
  "EA6", "BALEARIC Is.", 39.5, 3,
  "EA8", "CANARY Is.", 28, -15.5,
  "EA9", "MELILLA & CEUTA", 35, -3,
  "EI", "IRELAND", 53, -6,
  "EK", "ARMENIA", 41, 45,
  "EL", "LIBERIA", 7, -11,
  "EP", "IRAN", 36, 52,
  "ER", "MOLDAVIA", 47, 29,
  "ES", "ESTONIA", 49, 25,
  "ET", "ETHIOPIA", 9, 39,
  "EU", "BIELORUSSIA", 54, 28,
  "EX", "KIRGHISTAN", 42.9, 75,
  "EY", "TADJIKISTAN", 39.5, 68,
  "EZ", "TURKMENISTAN", 38, 58,
  "F", "FRANCE", 49, 2,
  "FG", "GUADELOUPE Is.", 16, -62,
  "FH", "MAYOTTE Is.", -13, 45,
  "FJ", "ST. BARTHELEMY", 48.818, 3.37,
  "FK", "NEW CALEDONIA", -22, 167,
  "FM", "MARTINIQUE", 15, -61,
  "FO", "POLYNESIA", -18, -150,
  "FO0", "CLIPPERTON Is.", 10.3, -109.25,
  "FP", "ST PIERRE & MIQUELON Is.", 47, -57,
  "FR", "REUNION", -21, 54,
  "FRG", "GLORIOSO Is.", -11, 47,
  "FRJ", "JUAN DE NOVA Is.", -17, 42,
  "FRT", "TROMELIN Is.", -16, 54,
  "FS", "ST MARTIN", 18, -63,
  "FT5W", "CROZET Is.", -46, 52,
  "FT5X", "KERGUELEN Is.", -49.25, 69.2,
  "FT5Z", "AMSTERDAM Is.", -38, 77.6,
  "FW", "WALLIS & FUTUNA", -13.3, -176.2,
  "FY", "FRENCH GUYANA", 5, -52,
  "G", "ENGLAND", 51.5, 0.1246,
  "GX", "ENGLAND", 51.5, 0.1246,
  "M", "ENGLAND", 51.5, 0.1246,
  "GD", "ISLE OF MAN", 54.25, -4.5,
  "GI", "NORTHERN IRELAND", 55, -6,
  "GJ", "JERSEY Is.", 49.25, -2.15,
  "GM", "SCOTLAND", 55.470, -2.271,
  "GU", "GUERNSEY Is.", 49.5, -2.6,
  "GW", "WALES", 53, -4,
  "H4", "SOLOMON Is.", -10, 160,
  "H40", "TEMOTU PROVINCE", -10.693, 165.946,
  "HA", "HUNGARY", 48, 19,
  "HB", "SWITZERLAND", 46.95, 7.45,
  "HB0", "LIECHTENSTEIN", 47.08, 9.6,
  "HC", "ECUADOR", 0, -78,
  "HC8", "GALAPAGOS Is.", -0.5, -91,
  "HH", "HAITI", 19, -72,
  "HI", "DOMINICAN REPUBLIC", 19, -70,
  "HK", "COLOMBIA", 5, -74,
  "HK0M", "MALPELO Is.", 4, -81.5,
  "HK0S", "SAN ANDRES Is.", 12.5, -81,
  "HL", "SOUTH KOREA", 37, 127,
  "HP", "PANAMA", 9, -80,
  "HR", "HONDURAS", 14, -87,
  "HS", "THAILAND", 14, 101,
  "HV", "VATICAN CITY", 42, 13,
  "HZ", "SAUDI ARABIA", 26, 50,
  "I", "ITALY", 42, -10,
  "IS", "SARDINIA Is.", 40, 9,
  "J2", "DJIBOUTI", 11.5, 43,
  "J3", "GRENADA", 12.1, -61.6,
  "J5", "GUINEA-BISSAU", 12, -15,
  "J6", "ST. LUCIA Is.", 14, -61,
  "J7", "DOMINICA Is.", 15.4, -61.35,
  "J8", "ST VINCENT Is.", 13.15, -61.1,
  "JA", "JAPAN", 35.7, 139.7,
  "JD1M", "MINAMI-TORISHIMA", 28.3, 154,
  "JD1O", "OGASAWARA", 27, 142.15,
  "JT", "MONGOLIA", 48, 107,
  "JW", "SVALBARD Is.", 78, 16,
  "JX", "JAN MAYEN Is.", 71, -8,
  "JY", "JORDAN", 32, 36,
  "KC4", "ANTARCTICA", -74.555, -0.63,
  "KC6", "BELAU", 5, 137,
  "KG4", "GUANTANAMO BAY", 20, -75,
  "KH0", "MARIANA Is.", 16, 146,
  "KH1", "BAKER & HOWLAND Is.", 1, -176,
  "KH2", "GUAM Is.", 14, 145,
  "KH3", "JOHNSTON Is.", 17, -170,
  "KH4", "MIDWAY Is.", 28.2, -177.25,
  "KH5", "PALMYRA Is.", 6, -162,
  "KH5K", "KINGMAN REEF", 7.5, -162.8,
  "KH6", "HAWAII", 21.3, -157.9,
  "KH7", "KURE Is.", 28.4, -178.8,
  "KH8", "AMERICAN SAMOA", -14, -171,
  "KH9", "WAKE Is.", 19.3, 167,
  "KL7", "ALASKA", 61.214744595031135, -149.83936437500003,
  "KP1", "NAVASSA Is.", 18.4, -75,
  "KP2", "VIRGIN Is.", 18.3, -64.7,
  "KP4", "PUERTO RICO", 18.5, -66.2,
  "KP5", "DESECHEO", 18.3, -67.5,
  "LA", "NORWAY", 60, 11,
  "LU", "ARGENTINA", -35, -58,
  "LX", "LUXEMBOURG", 49.75, 6.08,
  "LY", "LITHUANIA", 56, 26,
  "LZ", "BULGARIA", 43, 23,
  "OA", "PERU", -12.066160179273194, -76.97803625000002,
  "OD", "LEBANON", 34, 36,
  "OE", "AUSTRIA", 48.2, 16.4,
  "OH", "FINLAND", 62, 27,
  "OH0", "ALAND Is.", 60, 20,
  "OJ0", "MARKET REEF", 60, 21,
  "OK", "CZECH REPUBLIK", 50, 16,
  "OM", "SLOVAKIA", 49, 20,
  "ON", "BELGIUM", 51.0, 4.0,
  "OU", "DENMARK", 55.721, 12.282,
  "OX", "GREENLAND", 63, -52,
  "OY", "FAEROE Is.", 62, -7,
  "OZ", "DENMARK", 55.721, 12.282,
  "P2", "PAPUA NEW GUINEA", -5, 146,
  "P4", "ARUBA Is.", 12.5, -70,
  "P5", "NORTH KOREA", 39, 125.7,
  "PA", "NETHERLANDS", 52, 5,
  "PJ2", "NETH. ANTILLES", 12, -69,
  "PJ25", "CURACAO", 12.124, -68.871,
  "PJ4", "BONAIRE", 12.143, -68.266,
  "PJ5", "SABE-ST.EISTATIUS", 17.638, -63.239,
  "PJ7", "ST MAARTEN", 18, -63,
  "PY", "BRAZIL", -15.85040594997889, -47.84229406249998,
  "PY0F", "FERNANDO DE NORONHA Is.", -3.9, -32.5,
  "PY0S", "ST PETER-PAUL'S ROCKS", 2, -30,
  "PY0T", "TRINIDADE Is.", -21, -33,
  "PZ", "SURINAM", 6, -55,
  "R1FJ", "FRANZ JOSEF LAND", 80, 55,
  "R1MV", "MALYJVYSOTSKIJ Is.", 60.5, 28.5,
  "S0", "WESTERN SAHARA", 27, -13,
  "S2", "BANGLADESH", 24, 91,
  "S5", "SLOVENIA", 46, 14.5,
  "S7", "SEYCHELLES Is.", -4, 56,
  "S9", "SAO TOME", 0, 6,
  "SA", "SWEDEN", 59.5, 18,
  "SM", "SWEDEN", 59.5, 18,
  "SP", "POLAND", 51, 20,
  "ST", "SUDAN", 16, 33,
  "ST0", "SOUTHERN SUDAN", 5, 31.5,
  "SU", "EGYPT", 30, 31,
  "SV", "GREECE", 38, 24,
  "SV2ASP/A", "MT ATHOS", 40.2, 24.3,
  "SV5", "DODECANESE Is.", 36.5, 28,
  "SV9", "CRETE", 35, 25,
  "T2", "TUVALU", -9, 179,
  "T30", "WEST KIRIBATI", -1.5, 173,
  "T31", "CENTRAL KIRIBATI", -4, -172,
  "T32", "EAST KIRIBATI", 1.5, -157.5,
  "T33", "BANABA Is.", -1, 169.5,
  "T5", "SOMALIA", 2.1623, 45.17,
  "T7", "SAN MARINO", 44, 13,
  "T8", "PALAU", 7.515, 134.506,
  "T9", "BOSNIA", 43.5, 18.5,
  "TA", "TURKEY", 40, 33,
  "TF", "ICELAND", 64, -22,
  "TG", "GUATEMALA", 15, -91,
  "TI", "COSTA RICA", 10, -84,
  "TI9", "COCOS Is.", 6, -87,
  "TJ", "CAMEROON", 4, 12,
  "TK", "CORSICA", 42, 9,
  "TL", "CENTRAL AFR REP", 5, 18,
  "TN", "CONGO", -4, 15,
  "TR", "GABON REP", 1, 9,
  "TT", "CHAD REP", 12, 15,
  "TU", "IVORY COAST", 6, -4,
  "TX", "CHESTERFIELD IS.", 0.003, -0.012,
  "TY", "BENIN", 6, 2,
  "TZ", "MALI REP", 13, 8,
  "UA", "RUSSIA", 55.5, 38,
  "UA2", "KALININGRAD", 55, 21,
  "UA9", "ASIATIC RUSSIA", 55, 83,
  "UA0", "ASIATIC RUSSIA", 56, 93,
  "UK", "UZBEK", 40, 67,
  "UN", "KAZAKH", 50, 73,
  "UT", "UKRAINE", 51, 31,
  "V2", "ANTIGUA", 17, -60,
  "V3", "BELIZE", 18, -88,
  "V4", "ST KITTS", 17, -63,
  "V5", "NAMIBIA", -23, 16,
  "V6", "MICRONESIA", 7, 152,
  "V7", "MARSHALL Is.", 9.5, 167,
  "V8", "BRUNEI", 5, 115,
  "VE1", "NS - Halifax", 44.6, -63.6,
  "VE2", "QC - Montreal", 45.5, -73.6,
  "VE3", "ONT - Toronto", 43.7, -79.4,
  "VE3", "ONT - Thunder Bay", 48.4, -89.2,
  "VE4", "MAN - Winnipeg", 49.9, -97.1,
  "VE5", "SASK - Regina", 50.5, -104.6,
  "VE6", "ALTA - Calgary", 51, -114.1,
  "VE6", "ALTA - Edmonton", 53.5, -113.5,
  "VE7", "BC - Vancouver", 49.3, -123.1,
  "VE8", "NWT - Yellowknife", 62.5, -114.4,
  "VE9", "NB - St. John", 45.2, -66.1,
  "VO1", "NEWFOUNDLAND - ST.John's", 47.6, -52.7,
  "VO2", "LABRADOR - Goose Bay", 51.3, -60.4,
  "VK1", "AUSTRALIA - Canberra", -35.5, 149,
  "VK2", "AUSTRALIA - Sydney", -34, 151,
  "VK3", "AUSTRALIA - Melbourne", -37.87053090025539, 144.98973718749997,
  "VK4", "AUSTRALIA - Brisbane", -27.5, 153,
  "VK5", "AUSTRALIA - Adelaide", -34.9, 138.6,
  "VK6", "AUSTRALIA - Perth", -32, 115.9,
  "VK7", "AUSTRALIA - Hobart", -42.9, 147.3,
  "VK8", "AUSTRALIA - Darwin", -12.5, 130.9,
  "VK9C", "COCOS Is.", -12, 97,
  "VK9L", "LORD HOWE Is.", -32, 159,
  "VK9M", "MELLISH REEF", -17, 156,
  "VK9N", "NORFOLK Is.", -29, 168,
  "VK9X", "CHRISTMAS Is.", -11, 106,
  "VK9W", "WILLIS Is.", -16, 150,
  "VK0H", "HEARD Is.", -53, 73.5,
  "VK0M", "MACQUARIE Is.", -55, 159,
  "VP2E", "ANGUILLA", 18, -63,
  "VP2M", "MONTSERRAT", 17, -62,
  "VP2V", "BRITISH VIRGIN Is.", 18.5, -65,
  "VP5", "TURKS & CAICOS Is.", 22, -75,
  "VP6", "PITCAIRN Is.", -24.341, -128.35,
  "VP8", "FALKLAND Is.", -52, -59,
  "VP8G", "S GEORGIA Is.", -54.25, -36.7,
  "VP8O", "S ORKNEY Is.", -60, -46,
  "VP8S", "S SHETLAND Is.", -57, -28,
  "VP8W", "S SANDWICH Is.", -57.75, -26.5,
  "VP9", "BERMUDA", 32, -65,
  "VQ9", "CHAGOS - DIEGO GARCIA", -7, 72,
  "VS6", "HONG KONG", 23, 114,
  "VU", "INDIA", 28, 77,
  "VU4", "ANDAMAN & NICOBAR Is.", 12, 92.75,
  "VU7", "LACCADIVE Is.", 10, 73,
  "VY0", "NUNAVUT - Iqaluit", 63.75, -68.51,
  "VY1", "YUKON - Whitehorse", 60.7, -135.1,
  "VY2", "PEI - Charlottetown", 46.24, -63.13,
  "W1C", "CONN - Hartford", 41.8, -72.7,
  "W1M", "MAINE - Bangor", 44.8, -68.8,
  "W1B", "MASS - Boston", 42.4, -71.1,
  "W1N", "NH - Concord", 43.2, -71.5,
  "W1R", "RI - Providence", 41.8, -71.4,
  "W1V", "VT - Montpelier", 44.3, -72.6,
  "W2NJ", "NJ - Trenton", 40.1, -74.8,
  "W2N", "NY - New York City", 40.8, -74,
  "W2NY", "NY - Buffalo", 42.9, -78.9,
  "W3D", "DEL - Wilmington", 39.7, -75.5,
  "W3M", "MD - Baltimore", 39.3, -76.6,
  "W3P", "PENN - Philadelphia", 40, -75.2,
  "W3I", "PENN - Pittsburgh", 40.4, -80,
  "W3W", "Washington DC", 38.9, -77,
  "W4A", "ALA - Montgomery", 32.4, -86.3,
  "W4F", "FLA - Miami", 25.7, -80.3,
  "W4G", "GA - Atlanta", 33.7, -84.4,
  "W4K", "KY - Loiusville", 38.2, -85.8,
  "W4N", "NC - Raleigh", 35.8, -78.7,
  "W4S", "SC - Columbia", 34, -81,
  "W4T", "TENN - Knoxville", 35.9, -84,
  "W4E", "TENN - Memphis", 35.1, -90.1,
  "W4V", "VA - Richmond", 37.5, -77.5,
  "W5A", "ARK - Little Rock", 34.7, -92.4,
  "W5L", "LA - New Orleans", 30, -90.1,
  "W5M", "MISS - Jackson", 32.3, -90.3,
  "W5N", "NM - Albuquerque", 35.2, -106.7,
  "W5O", "OKLA - Oklahoma City", 35.5, -97.6,
  "W5T", "TEXAS - Dallas", 32.8, -96.8,
  "W5H", "TEXAS - Houston", 29.7, -95.4,
  "W5S", "TEXAS - San Antonio", 29.4, -98.5,
  "W6L", "CAL - Los Angeles", 34.1, -118.2,
  "W6D", "CAL - San Diego", 32.7, -117.2,
  "W6F", "CAL - San Fransisco", 37.7, -121.5,
  "W7A", "ARIZ - Phoenix", 33.5, -112.1,
  "W7I", "IDAHO - Boise", 43.6, -116.2,
  "W7M", "MONT - Helena", 46.6, -112,
  "W7N", "NEV - Las Vegas", 36.2, -115.2,
  "W7O", "OREG - Portland", 45.5, -122.7,
  "W7U", "UTAH - Salt Lake", 40.8, -111.9,
  "W7S", "WASH - Seattle", 47.5, -122.4,
  "W7P", "WASH - Spokane", 47.6, -117.5,
  "W7W", "WYO - Cheyenne", 41.2, -104.8,
  "W8M", "MICH - Detroit", 42.4, -83.1,
  "W8C", "OHIO - Cincinnati", 39.2, -84.6,
  "W8O", "OHIO - Cleveland", 41.5, -81.7,
  "W8W", "WVA - Charleston", 38.4, -81.7,
  "W9L", "ILL - Chicago", 41.8, -87.7,
  "W9I", "IND - Indianapolis", 39.7, -86.2,
  "W9W", "WISC - Milwaukee", 43, -87.9,
  "W0C", "COLO - Denver", 39.7, -104.9,
  "W0I", "IOWA - Des Moines", 41.6, -93.6,
  "W0W", "KANSAS - Wichita", 37.7, -97.4,
  "W0M", "MINN - Minneapolis", 45, -93.3,
  "W0K", "MO - Kansas City", 39.1, -94.6,
  "W0S", "MO - St. Louis", 38.7, -90.3,
  "W0N", "NEBR - Omaha", 41.3, -96,
  "W0ND", "ND - Bismarck", 46.8, -100.8,
  "W0P", "SD - Pierre", 44.4, -100.4,
  "XA", "MEXICO", 19, -99,
  "XE", "MEXICO", 19, -99,
  "XF4", "REVILLA GIGEDO", 18, -112,
  "XT", "BURKINA FASO", 12, -2,
  "XU", "CAMBODIA", 12, 105,
  "XW", "LAOS", 18, 103,
  "XX", "MACAO", 22, 114,
  "XZ", "MYANMAR", 17, 96,
  "YA", "AFGHANISTAN", 34, 69,
  "YB", "INDONESIA", -6, 107,
  "YI", "IRAQ", 33, 45,
  "YJ", "VANUATU", -16, 168,
  "YK", "SYRIA", 34, 37,
  "YL", "LATVIA", 57, 24,
  "YN", "NICARAGUA", 12, -86,
  "YO", "ROMANIA", 44.5, 26,
  "YS", "EL SALVADOR", 14, -89,
  "YT", "SERBIA", 46, 21,
  "YU", "SERBIA", 46, 21,
  "YV", "VENEZUELA", 11, -67,
  "YV0", "AVES Is.", 16, -63.5,
  "Z2", "ZIMBABWE", -18, 31,
  "Z3", "MACEDONIA", 42, 21.5,
  "Z6", "REP. OF KOSOVO", 42.666, 21.17,
  "Z8", "SOUTH SUDAN", 7.303, 31.179,
  "ZA", "ALBANIA", 42, 20,
  "ZB", "GIBRALTAR", 36, -6,
  "ZC4", "CYPRUS BR. BASES", 35, 33,
  "ZC6", "PALESTINIA", 32, 34.5,
  "ZD7", "ST HELENA Is.", -16, -6,
  "ZD8", "ASCENSION Is.", -8, -14,
  "ZD9", "TRISTAN DA CUNHA Is.", -37.25, -12.5,
  "ZF", "CAYMAN Is.", 20, -81,
  "ZK1N", "N COOK Is.", -11, -161,
  "ZK1S", "S COOK Is.", -21, -160,
  "ZK2", "NIUE", -19, -170,
  "ZK3", "TOKELAUS", -9, -171.75,
  "ZL", "NEW ZEALAND", -41, 175,
  "ZL7", "CHATHAM Is.", -44, -176.5,
  "ZL8", "KERMADEC Is.", -29.26, -177.9,
  "ZL9", "AUCKLAND & CAMPBELL Is.", -50.6, 166.3,
  "ZP", "PARAGUAY", -24, -60,
  "ZR", "SOUTH AFRICA", -27, 28,
  "ZS", "SOUTH AFRICA", -27, 28,
  "ZS8", "MARION & P.E. Is.", -46.6, 37.56,
  "", "", 0.0, 0.0  // EOT
};



/*****
  Purpose: To draw the onscreen keyboard and prompts

  Parameter list:
    void

  Return value:
    void
*****/
void DrawKeyboard() {
  int i;
  int keyWidth = 60;
  int keySpace = 10;
  int keyHeight = 40;
  int horizontalSpacer;
  int row;
  int xOffset;
  int keyCell;  // Where to place the cell in the X axis

  tft.clearMemory();  // Need to clear overlay too
  tft.writeTo(L2);
  tft.fillWindow();
  tft.writeTo(L1);
  tft.clearScreen(RA8875_BLACK);

  tft.setFontScale((enum RA8875tsize)1);
  tft.drawRect(30, 150, 730, 280, RA8875_WHITE);  // Draw frame
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);

  MyDelay(100L);
  row = 160;
  xOffset = 50;
  keyCell = 0;
  for (i = 0; i < 10; i++) {  // Numeric keys
    horizontalSpacer = xOffset + keyCell * (keyWidth + keySpace);
    tft.drawRect(horizontalSpacer, row, keyWidth, keyHeight, RA8875_YELLOW);
    tft.setCursor(horizontalSpacer + 24, row + 5);
    tft.print(letters[i + 28]);
    keyCell++;
  }

  row = 240;
  xOffset = 70;
  keyCell = 0;
  for (i = 0; i < 27; i++) {  // Alpha keys
    horizontalSpacer = xOffset + keyCell * (keyWidth + keySpace);
    tft.drawRect(horizontalSpacer, row + 5, keyWidth, keyHeight, RA8875_YELLOW);
    tft.setCursor(horizontalSpacer + 24, row + 7);
    if (i == 26) {
      tft.setCursor(horizontalSpacer + 20, row + 7);
      tft.print("SP");
      break;
    }
    tft.print((char)('A' + i));
    keyCell++;
    if (i + (int)'A' == 73) {
      row += 60;
      xOffset = 70;
      keyCell = 0;
    }
    if (i + 'A' == 82) {
      row += 60;
      xOffset = 70;
      keyCell = 0;
    }
  }
  tft.setFontScale((enum RA8875tsize)1);  // How to move cursor
  tft.setCursor(600, 0);
  tft.print("Menu+");
  tft.setFontScale((enum RA8875tsize)0);
  tft.setCursor(630, 35);
  tft.print("^");
  tft.setCursor(630, 45);
  tft.print("|");
  tft.setFontScale((enum RA8875tsize)1);
  tft.setCursor(500, 55);
  tft.print("Zoom <-   -> Band-");
  tft.setFontScale((enum RA8875tsize)0);
  tft.setCursor(630, 90);
  tft.print("|");
  tft.setCursor(630, 100);
  tft.print("v");
  tft.setFontScale((enum RA8875tsize)1);
  tft.setCursor(600, 115);
  tft.print("Demod");
  tft.setFontScale((enum RA8875tsize)1);

  tft.setFontScale((enum RA8875tsize)1);  // Prompts
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(10, 0);
  tft.printf("Select: Enter highlighted char ");
  tft.setCursor(10, 30);
  tft.printf(" User3: Done");
  tft.setCursor(10, 100);
  tft.printf("Enter call prefix: ");
}


/*****
  Purpose: To process the user keystrokes for the onscreen keyboard

  Parameter list:
    void

  Return value:
    void
*****/
void CaptureKeystrokes() {
  int keyWidth = 60;
  int keySpace = 10;
  int keyHeight = 40;
  int bufferIndex = 0;
  int horizontalSpacer;
  int row;
  int xOffset;
  int keyCell;  // Where to place the cell in the X axis
  int valPin;
  int whichLetterIndex;
  int pushButtonSwitchIndex;

  // Start at letter N

  row = 300;
  xOffset = 0;
  keyCell = 5;
  whichLetterIndex = 14;
  keyboardBuffer[0] = '\0';  // Clear buffer
  tft.setTextColor(RA8875_WHITE, RA8875_BLUE);
  DrawActiveLetter(row, spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
  while (true) {
    valPin = ReadSelectedPushButton();  // Poll UI push buttons
    MyDelay(150L);
    if (valPin != BOGUS_PIN_READ) {                        // If a button was pushed...
      pushButtonSwitchIndex = ProcessButtonPress(valPin);  // Winner, winner...chicken dinner!
      switch (pushButtonSwitchIndex) {
        case MENU_OPTION_SELECT:  // They selected a letter
          MyDelay(150L);
          if (row < 240) {
            keyboardBuffer[bufferIndex] = whichLetterIndex;
          } else {
            keyboardBuffer[bufferIndex] = letters[whichLetterIndex];
          }
          bufferIndex++;
          keyboardBuffer[bufferIndex] = '\0';  // Make it a string
          tft.setCursor(320, 100);
          tft.print(keyboardBuffer);
          break;

        case MAIN_MENU_UP:  // Go up a row
          if (row <= 155)   // Trying to go up above numerics
            break;
          DrawNormalLetter(row, spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
          MyDelay(150L);
          row -= 60;
          if (row < 240) {          // Move up to number line
            whichLetterIndex = 53;  // Move to 5
            row = 155;
            xOffset = 50;
            keyCell = 5;
            tft.setTextColor(RA8875_WHITE, RA8875_BLUE);
            horizontalSpacer = xOffset + keyCell * (keyWidth + keySpace);  // Restore current letter
            tft.fillRect(horizontalSpacer, row + 5, keyWidth, keyHeight, RA8875_BLUE);
            tft.setCursor(horizontalSpacer + 24, row + 7);
            tft.print((char)whichLetterIndex);
          } else {
            whichLetterIndex -= 9;
            tft.setTextColor(RA8875_WHITE, RA8875_BLUE);
            DrawActiveLetter(row, spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
          }
          break;

        case ZOOM:                       // Go left a column
          if (whichLetterIndex == 27) {  // For apace character
            tft.fillRect(spacing[keyCell], row + 5, keyWidth, keyHeight, RA8875_BLACK);
            tft.drawRect(spacing[keyCell], row + 5, keyWidth, keyHeight, RA8875_YELLOW);
            tft.setCursor(spacing[keyCell] + 20, row + 7);
            tft.print("SP");
            keyCell--;
            whichLetterIndex--;
            DrawActiveLetter(row, spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
          } else {
            if (row < 240) {       // Numberics
              if (keyCell == 0) {  // On 0, move to 9
                DrawNormalLetter(row, 50, whichLetterIndex, keyWidth, keyHeight);
                keyCell = 9;
                whichLetterIndex = 57;
                horizontalSpacer = 50 + keyCell * (keyWidth + keySpace);  // Restore current letter
                DrawActiveLetter(row, horizontalSpacer, whichLetterIndex, keyWidth, keyHeight);
              } else {
                horizontalSpacer = 50 + keyCell * (keyWidth + keySpace);  // Restore current letter
                DrawNormalLetter(row, horizontalSpacer, whichLetterIndex, keyWidth, keyHeight);
                keyCell--;
                horizontalSpacer = 50 + keyCell * (keyWidth + keySpace);  // Restore current letter
                whichLetterIndex--;
                DrawActiveLetter(row, horizontalSpacer, whichLetterIndex, keyWidth, keyHeight);
              }
            } else {
              DrawNormalLetter(row, spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
              keyCell--;
              whichLetterIndex--;
              if (keyCell == 0) {
                keyCell = 9;
                whichLetterIndex += 9;
              }
              DrawActiveLetter(row, spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
            }
          }
          break;

        case BAND_DN:                    // Go right one column
          if (whichLetterIndex == 27) {  // The space key
            tft.fillRect(spacing[keyCell], row + 5, keyWidth, keyHeight, RA8875_BLACK);
            tft.drawRect(spacing[keyCell], row + 5, keyWidth, keyHeight, RA8875_YELLOW);
            tft.setCursor(spacing[keyCell] + 20, row + 7);
            tft.print("SP");
            keyCell = 1;
            whichLetterIndex = 19;
          } else {
            if (row < 240) {  // In numerics row?
              xOffset = 50;   // Yep
            } else {
              xOffset = 0;  // Nope
            }
            DrawNormalLetter(row, xOffset + spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
            whichLetterIndex++;
            keyCell++;
          }
          if (keyCell > 9) {  // Falling off the end?
            if (row < 240) {  // In numerics row?
              xOffset = 50;   // Yep
              whichLetterIndex = 48;
              DrawActiveLetter(row, xOffset, whichLetterIndex, keyWidth, keyHeight);
              keyCell = 0;
              break;
            } else {
              xOffset = 0;  // Nope
            }
            keyCell = 1;
            whichLetterIndex -= 9;
            DrawActiveLetter(row, xOffset + spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
          }
          DrawActiveLetter(row, xOffset + spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
          break;

        case DEMODULATION:  // Go down a row
          if (row >= 360)
            break;
          if (row < 240) {
            xOffset = 50;
            horizontalSpacer = xOffset + keyCell * (keyWidth + keySpace);  // Restore current letter
            DrawNormalLetter(row, horizontalSpacer, whichLetterIndex, keyWidth, keyHeight);
            whichLetterIndex = 5;  // Move to 'W'
            row = 240;
            keyCell = 5;
            DrawActiveLetter(row, spacing[whichLetterIndex], whichLetterIndex, keyWidth, keyHeight);
          } else {
            DrawNormalLetter(row, spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
            row += 60;
            if (row > 360) {
              row -= 60;
              break;
            }
            whichLetterIndex += 9;
            DrawActiveLetter(row, spacing[keyCell], whichLetterIndex, keyWidth, keyHeight);
          }
          break;

        case UNUSED_1:  // Erase last entry
          delay(200L);
          bufferIndex--;                       // Move back in the buffer
          keyboardBuffer[bufferIndex] = '\0';  // Make it a string
          tft.setCursor(320, 100);
          tft.print("       ");  // erase old letter
          tft.setCursor(320, 100);
          tft.print(keyboardBuffer);
          break;

        case BEARING:  // They are all finished
          return;

        default:
          //          DrawActiveLetter(row, spacing[whichLetterIndex], whichLetterIndex, keyWidth, keyHeight);
          break;
      }
    }
  }
}

/*****
  Purpose: To draw the letters for the onscreen keyboard

  Parameter list:
    int row                   the screen row for this letter
    int horizontalSpacer      two characters for spacebar
    int whichLetterIndex      which letter is being drawn from the letters[] array
    int keyWidth,             the width of the drawn key
    int keyHeight                 height

  Return value:
    void
*****/
void DrawNormalLetter(int row, int horizontalSpacer, int whichLetterIndex, int keyWidth, int keyHeight) {
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.fillRect(horizontalSpacer, row + 5, keyWidth, keyHeight, RA8875_BLACK);
  tft.drawRect(horizontalSpacer, row + 5, keyWidth, keyHeight, RA8875_YELLOW);
  if (letters[whichLetterIndex] == '*') {  // Space
    tft.setCursor(horizontalSpacer + 20, row + 7);
    tft.print("SP");
  } else {
    if (row < 240) {
      horizontalSpacer += 22;
      tft.setCursor(horizontalSpacer, row + 7);
      tft.print((char)whichLetterIndex);
    } else {
      tft.setCursor(horizontalSpacer + 24, row + 7);
      tft.print(letters[whichLetterIndex]);
    }
  }
}

/*****
  Purpose: To draw the letters for the onscreen keyboard, but highlight it when drawn

  Parameter list:
    int row                   the screen row for this letter
    int horizontalSpacer      two characters for spacebar
    int whichLetterIndex      which letter is being drawn from the letters[] array
    int keyWidth,             the width of the drawn key
    int keyHeight                 height

  Return value:
    void
*****/
void DrawActiveLetter(int row, int horizontalSpacer, int whichLetterIndex, int keyWidth, int keyHeight) {
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.fillRect(horizontalSpacer, row + 5, keyWidth, keyHeight, RA8875_BLUE);
  if (letters[whichLetterIndex] == '*') {  // Space
    tft.setCursor(horizontalSpacer + 20, row + 7);
    tft.print("SP");
  } else {
    if (row < 240) {
      tft.setCursor(horizontalSpacer + 27, row + 7);
      tft.print((char)whichLetterIndex);
    } else {
      tft.setCursor(horizontalSpacer + 27, row + 7);
      tft.print(letters[whichLetterIndex]);
    }
  }
}

/*****
  Purpose: This function calculates the beam heading between your
           QTH and some other coordinates on the globe

  Argument:
    char *dxCallPrefix        the prefix of the station you're working

  return value
    double               the bearing heading to DX in degrees

 *****/
float BearingHeading(char *dxCallPrefix) {
  float deltaLong;  // For radians conversion
  float x, y;       // Temporary variables

  countryIndex = FindCountry(dxCallPrefix);  // do coutry lookup

  tft.clearScreen(RA8875_BLACK);

  if (countryIndex != -1) {              // Did we find prefix??
    dxLat = dxCities[countryIndex].lat;  //Yep, but I entered the
    dxLon = dxCities[countryIndex].lon;
  } else {
    return -1.0;
  }

  tft.setFontScale((enum RA8875tsize)1);
  deltaLong = (homeLon - dxLon);

  x = cos(dxLat * DEGREES2RADIANS) * sin(deltaLong * DEGREES2RADIANS);
  y = cos(homeLat * DEGREES2RADIANS) * sin(dxLat * DEGREES2RADIANS) - sin(homeLat * DEGREES2RADIANS) * cos(dxLat * DEGREES2RADIANS) * cos((deltaLong)*DEGREES2RADIANS);
  bearingDistance = HaversineDistance(dxLat, dxLon);


  bearingDegrees = atan2(x, y) * RADIANS2DEGREES;
  bearingDegrees = fmod(bearingDegrees, 360.0);

  if (bearingDegrees > 0) {
    displayBearing = 360.0 - bearingDegrees;
  } else {
    displayBearing = bearingDegrees * -1.0;
  }
  return displayBearing;
}
/*****
  Purpose: This function searches the ARRL list of country prefixes
           and returns an index into the cities array. The coordiantes
           are in degrees.

  Argument:
    char *prefix         the prefix of the station you're working

  return value
    int                  the index of the country, -1 if not found

 *****/
int FindCountry(char *prefix) {
  // callPrefix country  lat  lon
  int i = 0,
      index = -1,
      len,
      match;
  len = strlen(prefix);
  if (len == 1 && prefix[0] != 'B') {  // Only single letter prefix is China
    return -1;
  }

  while (true) {
    match = 1;
    for (index = 0; index < len; index++) {
      if (prefix[index] != dxCities[i].callPrefix[index]) {
        match = 0;
      }
    }
    if (match == 1) {  // All letters matched
      return i;
    }
    i++;
    if (dxCities[i].callPrefix[0] == '\0') {  // Searched the entire table
      return -1;
    }
  }
}


/*****
  Purpose: This function calculates the distance between your
           QTH and some other coordinates on the globe

  Argument:
    double homeLat    // Home values
    double homeLon
    double dxLat      // DX values
    double dxLon

  return value
    double               the distance in km. Multiply by 0.6213712 for miles

From: https://stackoverflow.com/questions/27928/calculate-distance-between-two-latitude-longitude-points-haversine-formula
*****/
float HaversineDistance(float lat2, float lon2) {
  const float r = 6371;  // Earth's radius in km
  const float p = PI_BY_180;
  float lat1, lon1;

  lat1 = homeLat;
  lon1 = homeLon;

  const float a = 0.5 - cos((lat2 - lat1) * p) / 2
                  + cos(lat1 * p) * cos(lat2 * p) * (1 - cos((lon2 - lon1) * p)) / 2;

  return 2.0 * r * asin(sqrt(a));
}
//======================================


void bmpDraw(const char *filename, int x, int y) {
  //  int image_width, image_height;        // W+H in pixels
  int len;
  int rayStart = 0, rayEnd = 0;
  File bmpFile;
  int bmpWidth, bmpHeight;             // W+H in pixels
  uint8_t bmpDepth;                    // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;             // Start of image data in file
  uint32_t rowSize;                    // Not always = bmpWidth; may have padding
  uint8_t sdbuffer[3 * BUFFPIXEL];     // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];       // pixel out buffer (16-bit per pixel)
  uint8_t buffidx = sizeof(sdbuffer);  // Current position in sdbuffer
  boolean goodBmp = false;             // Set to true on valid header parse
  boolean flip = true;                 // BMP is stored bottom-to-top
  int w, h, row, col, xpos, ypos;

  homeLat = myMapFiles[selectedMapIndex].lat;
  homeLon = myMapFiles[selectedMapIndex].lon;  // your QTH longitude

  uint8_t r, g, b;
  uint32_t pos = 0;
  uint8_t lcdidx = 0;
  float x1, y1, y2;
  float homeLatRadians;
  float dxLatRadians;
  float deltaLon = (dxLon - homeLon);
  float deltaLonRadians;

  if ((x >= tft.width()) || (y >= tft.height()))
    return;

  if (!SD.begin(chipSelect)) {
    tft.print("SD card cannot be initialized.");
    MyDelay(2000L);  // Given them time to read it.
    return;
  }
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == false) {
    tft.setCursor(100, 300);
    tft.print("File not found");
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) {  // BMP signature
    read32(bmpFile);
    (void)read32(bmpFile);             // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile);  // Start of image data

    // Read DIB header
    read32(bmpFile);
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);

    if (read16(bmpFile) == 1) {                          // # planes -- must be '1'
      bmpDepth = read16(bmpFile);                        // bits per pixel
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) {  // 0 = uncompressed
        goodBmp = true;                                  // Supported BMP format -- proceed!

        //                                                      BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width()) w = tft.width() - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        ypos = y;
        for (row = 0; row < h; row++) {  // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (flip)  // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else  // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;

          if (bmpFile.position() != pos) {  // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer);  // Force buffer reload
          }
          xpos = x;
          for (col = 0; col < w; col++) {  // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) {  // Indeed
              // Push LCD buffer to the display first
              if (lcdidx > 0) {
                tft.drawPixels(lcdbuffer, lcdidx, xpos, ypos);
                xpos += lcdidx;
                lcdidx = 0;
              }

              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0;  // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = Color565(r, g, b);
            if (lcdidx >= sizeof(lcdbuffer) || (xpos - x + lcdidx) >= w) {
              tft.drawPixels(lcdbuffer, lcdidx, xpos, ypos);
              lcdidx = 0;
              xpos += lcdidx;
            }
          }  // end pixel
          ypos++;
        }  // end scanline

        // Write any remaining data to LCD
        if (lcdidx > 0) {
          tft.drawPixels(lcdbuffer, lcdidx, xpos, ypos);
          xpos += lcdidx;
        }
      }  // end goodBmp
    }
  }
  bmpFile.close();
  if (!goodBmp) {
    tft.setCursor(100, 300);
    tft.print("BMP format not recognized.");
  }

  //======================================
  homeLatRadians = homeLat * DEGREES2RADIANS;
  dxLatRadians = dxLat * DEGREES2RADIANS;
  deltaLonRadians = deltaLon * DEGREES2RADIANS;

  float yR = sin(deltaLonRadians) * cos(dxLatRadians);
  float xR = cos(homeLatRadians) * sin(dxLatRadians) - sin(homeLatRadians) * cos(dxLatRadians) * cos(deltaLonRadians);

  bearingRadians = atan2(yR, xR);
  bearingDegrees = bearingRadians * RADIANS2DEGREES;

  x1 = CENTER_SCREEN_X;                                          // The image center coordinates
  y1 = CENTER_SCREEN_Y;                                          // and should be the same for all
  x2 = CENTER_SCREEN_X * sin(bearingRadians) + CENTER_SCREEN_X;  // Endpoints for ray
  y2 = CENTER_SCREEN_Y * cos(bearingRadians) - CENTER_SCREEN_Y;

  rayStart = displayBearing - 8.0;
  rayEnd = displayBearing + 8.0;

  if (displayBearing > 16 && displayBearing < 345) {  // Check for end-point mapping issues
    rayStart = -8;
    rayEnd = 9;
  } else {
    if (displayBearing < 9) {
      rayStart = 8 - displayBearing;
      rayEnd = displayBearing + 8;
    } else {
      if (displayBearing > 344) {
        rayStart = displayBearing - 8;
        rayEnd = displayBearing + 8;
      }
    }
  }
  if (y2 < 0) {
    y2 = fabs(y2);
  }

  for (int i = rayStart; i < rayEnd; i++) {
    tft.drawLineAngle(x1, y1, displayBearing + i, RAY_LENGTH, RA8875_RED, -90);
  }
  len = strlen(dxCities[countryIndex].country);
  tft.setCursor(380 - (len * tft.getFontWidth(0)) / 2, 1);  // Center city above image
  tft.setTextColor(RA8875_GREEN);

  tft.print(dxCities[countryIndex].country);
  tft.setCursor(20, 440);
  tft.print("Bearing:  ");
  tft.print(displayBearing, 0);
  tft.print(DEGREE_SYMBOL);
  tft.setCursor(480, 440);
  tft.print("  Distance: ");
  tft.print(bearingDistance, 0);
  tft.print(" km");
  //    tft.print(d * 0.6213712);   // If you want miles instead, comment out previous 2 lines and uncomment
  //    tft.print(" miles");        // these 2 lines


  bmpFile.close();
  if (!goodBmp) {
    tft.setCursor(100, 300);
    tft.print("BMP format not recognized.");
  }
}

/*****
  Purpose: The next two functions read 16- and 32-bit image data from the SD card file.
           BMP data is stored little-endian, Arduino is little-endian too.

  Parameter list:
    File &f            the lvalue of the image file handle

  Return value;
    uint16_t           the image data in proper format

  CAUTION: Other systems may not be little-endian so you may have to reverse subscripting
*****/
uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();  // MSB
  return result;
}
/*****
  Purpose: Read 32-bit image data from the SD card file.
           BMP data is stored little-endian, Arduino is little-endian too.

  Parameter list:
    File &f            the lvalue of the image file handle

  Return value;
    uint16_t           the image data in proper format

  CAUTION: Other systems may not be little-endian so you may have to reverse subscripting
*****/
uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();  // MSB
  return result;
}

/*****
  Purpose: Function takes the RBG color values and converts them for their BCD format to the
           equivalent tft color value.

  Parameter list:
    int8_t r      The RED component
    int8_t g      The GREEN component
    int8_t b      The BLUE component

  Return value:
    uint16_t      the two-byte tft equivalent colore
*****/
uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


//=============================================================================
// TFT Helper functions to work on ILI9341_t3
// which doe snot have offset/clipping support
//=============================================================================

#if !defined(TFT_CLIP_SUPPORT)

// helper to helper...
inline void writeRect(int x, int y, int cx, int cy, uint16_t *pixels) {
#if defined(_RA8876_T3)
  tft.useCanvas();
  tft.putPicture_16bpp(x, y, cx, cy);
  tft.startSend();
  SPI.transfer(RA8876_SPI_DATAWRITE);
  SPI.transfer(pixels, NULL, cx * cy * 2);
  tft.endSend(true);
#else
  tft.writeRect(x, y, cx, cy, pixels);
#endif
}

void writeClippedRect(int x, int y, int cx, int cy, uint16_t *pixels, bool waitForWRC) {
  x += g_image_offset_x;
  y += g_image_offset_y;
  int end_x = x + cx;
  int end_y = y + cy;

  if ((x >= 0) && (y >= 0) && (end_x <= g_tft_width) && (end_y <= g_tft_height)) {

#ifdef TFT_EMULATE_FB
    if (g_frame_buffer && g_use_efb) {

      uint16_t *pfb = &g_frame_buffer[y * g_tft_width + x];
      while (cy--) {
        memcpy(pfb, pixels, cx * 2);  // output one clipped rows worth
        pfb += g_tft_width;
        pixels += cx;
      }
    } else
#endif
    {
      writeRect(x, y, cx, cy, pixels);
    }

    g_WRCount++;

    if (waitForWRC) WaitforWRComplete();
    // only process if something is visible.
  } else if ((end_x >= 0) && (end_y >= 0) && (x < g_tft_width) && (y < g_tft_height)) {

    int cx_out = cx;
    int cy_out = cy;
    if (x < 0) {
      pixels += -x;  // point to first word we will use.
      cx_out += x;
      x = 0;
    }
    if (end_x > g_tft_width) cx_out -= (end_x - g_tft_width);
    if (y < 0) {
      pixels += -y * cx;  // point to first word we will use.
      cy_out += y;
      y = 0;
    }
    if (end_y > g_tft_height) cy_out -= (end_y - g_tft_height);
    if (cx_out && cy_out) {

#ifdef TFT_EMULATE_FB
      if (g_frame_buffer && g_use_efb) {
        uint16_t *pfb = &g_frame_buffer[y * g_tft_width + x];
        while (cy_out--) {
          memcpy(pfb, pixels, cx_out * 2);  // output one clipped rows worth
          pfb += g_tft_width;
          pixels += cx;
        }
      } else
#endif
      {

        if (cy_out > 1) {
          //compress the buffer
          uint16_t *pixels_out = pixels;
          uint16_t *p = pixels;
          end_y = cy_out;  // reuse variable
          while (--end_y) {
            p += cx_out;       // increment to where we will copy the pixels to
            pixels_out += cx;  // increment by one full row
            memcpy(p, pixels_out, cx_out * sizeof(uint16_t));
          }
        }
        writeRect(x, y, cx_out, cy_out, pixels);
      }
      g_WRCount++;

    } else {
#ifdef DEBUG
      Serial.println(" Clipped");
#endif
    }
  } else {
#ifdef DEBUG
    Serial.println(" Clipped");
#endif
  }
}

#endif

inline void Color565ToRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b) {
  tft.Color565ToRGB(color, r, g, b);
}


/*****
  Purpose: Initialize the SD card

  Parameter list:
    void

  Return value;
    int                   0 if cannot initialize, 1 otherwise
*****/
int InitializeSDCard() {
  tft.setFontScale((enum RA8875tsize)1);
  tft.setTextColor(RA8875_RED, RA8875_BLACK);
  tft.setCursor(100, 240);
  if (!SD.begin(chipSelect)) {
    tft.print("SD card cannot be initialized.");
    MyDelay(2000L);  // Given them time to read it.
    return 0;
  }

  //  tft.print("Initializing SD card.");
  //  MyDelay(2000L);
  return 1;
}

/*****
  Purpose: Erase initialization error message

  Parameter list:
    void

  Return value;
    int                   0 if cannot initialize, 1 otherwise
*****/
void TurnOffInitializingMessage() {
  tft.setFontScale((enum RA8875tsize)1);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.setCursor(100, 240);
  tft.print("                              ");
}
void WaitforWRComplete() {
#if defined(_RA8876_T3)
  // bugbug: ra8876 may use dma code, and since some of our decoders
  // want to reuse the same memory we wait for these to complete
  while (!tft.DMAFinished())
    ;
#endif
}

/*****
  Purpose: TO present a list of the bearing maps on the SD card.

  Parameter list:
    void

  Return value;
    int                   0 if cannot initialize, 1 otherwise
*****/
int BearingMaps() {
  char ptrMaps[10][50];
  int count;

  if (sdCardPresent == 0) {  // JJP 8/11/23
    tft.setCursor(200, 300);
    tft.setTextColor(RA8875_RED, RA8875_BLACK);
    tft.println("No SD card.");
    MyDelay(SDCARD_MESSAGE_LENGTH);
    tft.fillRect(200, 300, tft.getFontWidth() * 12, tft.getFontHeight(), RA8875_BLACK);
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    return 0;
  }
  count = CreateMapList(ptrMaps, &count);  // Reads the SD card for BMP files and returns ptrMaps filled in with names and return the count

  if (count == 0) {  // They have a card, but no maps
    tft.setCursor(300, 300);
    tft.print("No Maps found");
    selectedMapIndex = -1;
    return -1;  // Didn't find any
  }
  if (count == 1) {
    selectedMapIndex = 0;
  } else {
    tft.clearMemory();  // Need to clear overlay too
    tft.writeTo(L2);
    tft.fillWindow();

    tft.setFontScale((enum RA8875tsize)1);
    tft.drawRect(30, 50, 730, 400, RA8875_WHITE);  // Outline
    tft.setTextColor(RA8875_GREEN, RA8875_BLACK);
    tft.setCursor(50, 15);
    tft.print("Select map file:");
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);

    if (!SD.begin(chipSelect)) {
      tft.setCursor(200, 200);
      tft.setTextColor(RA8875_RED, RA8875_BLACK);
      tft.println("initialization failed!");
      MyDelay(5000L);
      tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
      return -1;
    }
    selectedMapIndex = WhichOneToUse(ptrMaps, count);
  }
  homeLat = myMapFiles[selectedMapIndex].lat;
  homeLon = myMapFiles[selectedMapIndex].lon;  // your QTH longitude

  strcpy(mapFileName, (const char *)myMapFiles[selectedMapIndex].mapNames);

  RedrawDisplayScreen();
  ShowFrequency();
  DrawFrequencyBarValue();

  return selectedMapIndex;
}


/*****
  Purpose: Read the files on the SD card and returns the number of BMP files found

  Parameter list:
    char ptrMaps[][]    array to hold the names of the BMP files that are found
    int *count          the number of files found

  Return value;
    int                   0 if cannot initialize, 1 otherwise
*****/
int CreateMapList(char ptrMaps[][50], int *count) {
  int index;
  int temp = 0;

  File root = SD.open("/");
  index = 0;
  *count = 0;
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      break;
    }

    tft.setCursor(50, 55 + temp * 30);
    if (strstr(entry.name(), ".bmp") != NULL) {
      strcpy(&ptrMaps[temp][0], entry.name());
      temp++;
    }

    if (!entry.isDirectory()) {
      index++;
    }
    *count = temp;
    entry.close();
  }
  return temp;
}

/*****
  Purpose: Read the files on the SD card and returns the number of BMP files found

  Parameter list:
    char ptrMaps[][]    array to hold the names of the BMP files that are found
    int *count          the number of files found

  Return value;
    int                   0 if cannot initialize, 1 otherwise
*****/
int WhichOneToUse(char ptrMaps[][50], int count) {
  int temp = 0;
  int i;
  int val;

  MyDelay(100L);
  for (i = 0; i < count; i++) {  // Yep.
    tft.setCursor(50, 55 + i * 30);
    tft.print(ptrMaps[i]);
  }

  tft.setCursor(50, 55);  // Shoiw first one in the list
  tft.setTextColor(RA8875_BLACK, RA8875_GREEN);
  tft.print(ptrMaps[0]);

  while (true) {
    if (filterEncoderMove != 0) {         // Did they move the encoder?
      tft.setCursor(50, 55 + temp * 30);  // Restore old highlighted name
      tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
      tft.print(ptrMaps[temp]);
      temp += (int)filterEncoderMove;
      if (temp > count - 1) {
        temp = 0;  // Wrap to the first in the list
      } else {
        if (temp < 0) {
          temp = count - 1;  // Wrap to end of list
        }
      }
      tft.setCursor(50, 55 + temp * 30);  // Highlight new name
      tft.setTextColor(RA8875_BLACK, RA8875_GREEN);
      tft.print(ptrMaps[temp]);
      filterEncoderMove = 0;
    }
    val = ReadSelectedPushButton();  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(100L);
    if (val == MENU_OPTION_SELECT) {  // Make a choice??
      break;                          // Yep.
    }
  }
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  return temp;
}