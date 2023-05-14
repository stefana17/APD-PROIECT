/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: On_Panel */
#define  PANEL_SWITCH_WAVE_FREQ           2       /* control type: binary, callback function: OnSwitchWaveFreq */
#define  PANEL_SAVE                       3       /* control type: command, callback function: On_Save */
#define  PANEL_LOAD                       4       /* control type: command, callback function: On_Load */
#define  PANEL_GRAPH_FILTRED_DATA         5       /* control type: graph, callback function: (none) */
#define  PANEL_GRAPH_RAW_DATA             6       /* control type: graph, callback function: (none) */
#define  PANEL_ZERO_CROSSING              7       /* control type: numeric, callback function: (none) */
#define  PANEL_MEDIANA                    8       /* control type: numeric, callback function: (none) */
#define  PANEL_MEDIE                      9       /* control type: numeric, callback function: (none) */
#define  PANEL_DISPERSIE                  10      /* control type: numeric, callback function: (none) */
#define  PANEL_MAXIM                      11      /* control type: numeric, callback function: (none) */
#define  PANEL_INDEX_MAXIM                12      /* control type: numeric, callback function: (none) */
#define  PANEL_INDEX_MINIM                13      /* control type: numeric, callback function: (none) */
#define  PANEL_MINIM                      14      /* control type: numeric, callback function: (none) */
#define  PANEL_HISTOGRAMA                 15      /* control type: command, callback function: On_Histograma */
#define  PANEL_NEXT                       16      /* control type: command, callback function: On_Next */
#define  PANEL_APLICA                     17      /* control type: command, callback function: On_Aplica */
#define  PANEL_PREVIOUS                   18      /* control type: command, callback function: On_Previous */
#define  PANEL_STOP                       19      /* control type: numeric, callback function: (none) */
#define  PANEL_ALPHA                      20      /* control type: numeric, callback function: (none) */
#define  PANEL_START                      21      /* control type: numeric, callback function: (none) */
#define  PANEL_FILTRU                     22      /* control type: ring, callback function: On_Filtru */
#define  PANEL_DIM_FEREASTRA              23      /* control type: numeric, callback function: (none) */
#define  PANEL_ANVELOPA                   24      /* control type: command, callback function: On_Avelopa */
#define  PANEL_DERIVATA                   25      /* control type: command, callback function: On_Derivata */
#define  PANEL_DECORATION                 26      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_2               27      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_3               28      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_4               29      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_5               30      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_6               31      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_7               32      /* control type: deco, callback function: (none) */

#define  PANEL_ANVE                       2       /* callback function: OnPanelAnvelopa */
#define  PANEL_ANVE_GRAPH_ANVELOPA        2       /* control type: graph, callback function: (none) */

#define  PANEL_DERI                       3       /* callback function: OnPanelDerivata */
#define  PANEL_DERI_GRAPH_DERIVATA        2       /* control type: graph, callback function: (none) */

#define  PANEL_FREQ                       4       /* callback function: OnPanelFreq */
#define  PANEL_FREQ_SWITCH_FILTER_TYPE    2       /* control type: binary, callback function: (none) */
#define  PANEL_FREQ_SWITCH_WINDOW_TYPE    3       /* control type: binary, callback function: (none) */
#define  PANEL_FREQ_SWITCH_WAVE_FREQ      4       /* control type: binary, callback function: OnSwitchWaveFreq */
#define  PANEL_FREQ_GRAPH_FERESTRUIRE     5       /* control type: graph, callback function: (none) */
#define  PANEL_FREQ_GRAPH_FILTRU          6       /* control type: graph, callback function: (none) */
#define  PANEL_FREQ_GRAPH_SPECTRU_2       7       /* control type: graph, callback function: (none) */
#define  PANEL_FREQ_GRAPH_RAW_SIGNAL_2    8       /* control type: graph, callback function: (none) */
#define  PANEL_FREQ_GRAPH_SPECTRU_1       9       /* control type: graph, callback function: (none) */
#define  PANEL_FREQ_FREQUENCY_PEAK_2      10      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_FREQUENCY_PEAK        11      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_POWER_PEAK_2          12      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_POWER_PEAK            13      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_SECUNDA               14      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_ORDIN                 15      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_ATENUARE              16      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_RIPPLE                17      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_DIM_FEREASTRA         18      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_SPECTRU               19      /* control type: command, callback function: OnSpectru */
#define  PANEL_FREQ_TIMER                 20      /* control type: timer, callback function: OnTimer */
#define  PANEL_FREQ_TIMERBUTTON           21      /* control type: textButton, callback function: OnActivareTimer */
#define  PANEL_FREQ_DECORATION            22      /* control type: deco, callback function: (none) */
#define  PANEL_FREQ_DECORATION_2          23      /* control type: deco, callback function: (none) */
#define  PANEL_FREQ_DECORATION_3          24      /* control type: deco, callback function: (none) */
#define  PANEL_FREQ_DECORATION_4          25      /* control type: deco, callback function: (none) */
#define  PANEL_FREQ_DECORATION_5          26      /* control type: deco, callback function: (none) */
#define  PANEL_FREQ_DECORATION_6          27      /* control type: deco, callback function: (none) */
#define  PANEL_FREQ_DECORATION_7          28      /* control type: deco, callback function: (none) */

#define  PANEL_HIST                       5       /* callback function: OnPanelHistograma */
#define  PANEL_HIST_GRAPH_HISTOGRAMA      2       /* control type: graph, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK On_Aplica(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Avelopa(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Derivata(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Filtru(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Histograma(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Load(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Next(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Previous(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Save(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnActivareTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelAnvelopa(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelDerivata(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelFreq(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelHistograma(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSpectru(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSwitchWaveFreq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif