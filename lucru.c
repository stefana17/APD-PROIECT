//==============================================================================
// Include files
#include <advanlys.h>
#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "lucru.h"

//==============================================================================
// Static global variables

static int panelHandle;        //panoul principal WaveData
static int panelHistograma;    //panoul pentru histrograma
//static int panelAnvelopa;    //panoul pentru anvelopa
static int panelDerivata;      //panoul pentru derivata
static int panelFrecventa;     //panoul pentru frecventa

//==============================================================================
// Global functions

int waveInfo[2]; //waveInfo[0] = sampleRate
				 //waveInfo[1] = number of elements
double frecventa = 0.0;
int npoints = 0;
double *waveData = 0;       //vectorul initial cu date
double *filtrare = 0;       //vectorul filtrat
double **secunde = 0;       //vectorul cu secunde
int intervalesec = 6;       //in cate intervale impart 
int npunctepersec = 0;      //cate puncte am eu pe secunda
double maxVal = 0.0;		//maximul
double minVal = 0.0;		//minimul
int am_apasat_load = 0;
int am_apasat_aplica = 0;
int nextsec = 0;            //imi mentine numaru secundei 
double alpha = 0.0;			//variabila folosita pentru filtrarea de ordin intai
int esantioane = 0.0;       //dimensiunea ferestrei -> folosit pentru filtrarea prin mediere
double **anvelopapersec = 0;
double* anvelopa = 0;
int am_apasat_anvelopa = 0;

/*******************************************************************************
	Functia main
*******************************************************************************/
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "lucru.uir", PANEL)) < 0)
		return -1;
	if ((panelFrecventa = LoadPanel (0, "lucru.uir", PANEL_FREQ)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	
	return 0;
}

/*******************************************************************************
	Functia de callback a butonului LOAD
*******************************************************************************/
int CVICALLBACK On_Load (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	int maxIdx, minIdx;
	double medie = 0.0;
	double dispersie = 0.0;
	double mediana = 0.0;
	double zero_crossing = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			//executa script python pentru conversia unui fisierului .wav in .txt
			//LaunchExecutable("python main.py");
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar)
			//Delay(4);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			FileToArray("waveInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			frecventa = waveInfo[0];
			npoints = waveInfo[1];
			
			//alocare memorie pentru numarul de puncte
			waveData = (double *) calloc(npoints, sizeof(double));
			
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			//afisare pe grapf
			PlotY(panel, PANEL_GRAPH_RAW_DATA, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
			
			am_apasat_load = 1;
			
			//minim
			MaxMin1D(waveData, npoints, &maxVal, &maxIdx, &minVal, &minIdx);
			SetCtrlVal(panel, PANEL_MINIM, minVal);
			SetCtrlVal(panel, PANEL_INDEX_MINIM, minIdx);
			
			//maxim
			MaxMin1D(waveData, npoints, &maxVal, &maxIdx, &minVal, &minIdx);
			SetCtrlVal(panel, PANEL_MAXIM, maxVal);
			SetCtrlVal(panel, PANEL_INDEX_MAXIM, maxIdx);
			
			//medie + dispersie
			Variance(waveData, npoints, &medie, &dispersie);
			SetCtrlVal(panel, PANEL_MEDIE, medie);
			SetCtrlVal(panel, PANEL_DISPERSIE, sqrt(dispersie));
			
			//mediana
			Median(waveData, npoints, &mediana);
			//printf("%lf", mediana);
			SetCtrlVal(panel, PANEL_MEDIANA, mediana);
			
			//zero_crossing
			for(int i = 0; i < npoints - 1; ++i)
			{
				if((waveData[i] < 0 && waveData[i+1] > 0) || (waveData[i] > 0 && waveData[i+1] < 0))
				{
					zero_crossing++;
				}
			}
			SetCtrlVal(panel, PANEL_ZERO_CROSSING, zero_crossing);
			
			npunctepersec = npoints / intervalesec;
			
			//alocare memorie linii 
			secunde = (double **) malloc(sizeof (double *) * intervalesec);
			for (int i = 0; i < intervalesec; ++i) 
			{
        		secunde[i] = (double*) malloc(sizeof (double) * npunctepersec); //alocare memorie coloane
    		}
			
			//construirea vectorului cu secunde   
			int count = 0; //numar de coloana -> adica in a cata coloana sunt
			for(int i = 0; i < intervalesec; ++i) //parcurg liniile -> adica fiecare secunda 0->1; 1->2 etc. 
			{
				for(int j = npunctepersec * i; j < npunctepersec * (i + 1); ++j) // parcurg vectorul initial(waveData) pentru a lua secundele din el
				{
					secunde[i][count] = waveData[j];
					count++;
				}
				count = 0;
			}
			
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a primului panou
*******************************************************************************/
int CVICALLBACK On_Panel (int panel, int event, void *callbackData,
						  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			if(waveData)
			{
				free(waveData);
			}
			if(filtrare)
			{
				free(filtrare);
			}
			if(secunde)
			{
				free(secunde);
			}
			/*if(intervalesec)
			{
				free(intervalesec);
			}*/
			QuitUserInterface(0);
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a butonului HISTOGRAMA
*******************************************************************************/
int CVICALLBACK On_Histograma (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	int intervals = 20; //numar intervale in care am impartit waveData 
	switch (event)
	{
		case EVENT_COMMIT:
		if(am_apasat_load)
		{
			ssize_t histogramArray [intervals];  //vector de frecventa
			double axisArray[intervals];    //axa X
		
			Histogram(waveData, npoints, minVal, maxVal, histogramArray, axisArray, intervals);
			
			panelHistograma = LoadPanel(1, "lucru.uir", PANEL_HIST);
			InstallPopup(panelHistograma);
			
			DeleteGraphPlot(panelHistograma, PANEL_HIST_GRAPH_HISTOGRAMA, -1, VAL_IMMEDIATE_DRAW );
			PlotXY(panelHistograma, PANEL_HIST_GRAPH_HISTOGRAMA, axisArray, histogramArray, intervals, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_CYAN);
		}
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a panoului pentru Histograma
*******************************************************************************/
int CVICALLBACK OnPanelHistograma (int panel, int event, void *callbackData,
								   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			RemovePopup(panelHistograma);
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia pentru filtrare mediere
*******************************************************************************/
double * filtrare_mediere (int numarpuncte, double * data)
{
	double * filtru_final = (double *) malloc (numarpuncte * sizeof(double));

	//copiez primele esantioane-1 valori -> primele 16 sau 32 de valori
	for (int i = 0 ; i < esantioane - 2 ; ++i){
		filtru_final[i] = data[i];
	}

	//de la esantioane incep sa fac filtrarea
	for(int i = esantioane ; i < numarpuncte ; ++i){
		double sum = 0.0;
		for(int j =  i - esantioane; j < i; ++j){
			sum += data[j];
}
		filtru_final[i] = sum / esantioane;
	}

	return filtru_final;
}

/*******************************************************************************
	Functia pentru filtrare de ordin intai
*******************************************************************************/
double * filtrare_ordinI (int numarpuncte, double * data) //numarpuncte -> ori npoints ori npunctepersec ; data -> ori waveData ori secunde[nextsec]
{
	double * filtru_final = (double *) malloc (numarpuncte * sizeof(double));

	filtru_final[0] = data[0]; 

	for(int i = 1; i < numarpuncte; ++i)
	{
		filtru_final[i] =  (1 - alpha) * filtru_final[i - 1] + alpha * data[i];
	}

	return filtru_final;
}

/*******************************************************************************
	Functia pentru filtrare 
*******************************************************************************/
void filtrare_plotare (int panel, double * data, int numarpuncte)
{
	int val;
	GetCtrlVal(panel, PANEL_FILTRU, &val);

	if(val == 0)
	{ 
		//filtrare prin mediere
		GetCtrlVal (panel, PANEL_DIM_FEREASTRA, &esantioane);
		filtrare = filtrare_mediere(numarpuncte, data);
		
		//facem plotare pe graful 2
		DeleteGraphPlot( panel, PANEL_GRAPH_FILTRED_DATA, -1, VAL_IMMEDIATE_DRAW );
		PlotY(panel, PANEL_GRAPH_FILTRED_DATA, filtrare, numarpuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
	}
	else
	{ 
		//filtrare cu element de ordin 1
		filtrare = filtrare_ordinI(numarpuncte, data);

		//facem plotare pe graful 2
		DeleteGraphPlot(panel, PANEL_GRAPH_FILTRED_DATA, -1, VAL_IMMEDIATE_DRAW );
		PlotY(panel, PANEL_GRAPH_FILTRED_DATA, filtrare, numarpuncte, VAL_DOUBLE,VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
	}
}

/*******************************************************************************
	Functia de callback a butonului Aplica
*******************************************************************************/
int CVICALLBACK On_Aplica (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, PANEL_ALPHA, &alpha);
			if(am_apasat_load == 1)
			{
				am_apasat_aplica = 1;
				if(nextsec == 0)
				{
					filtrare_plotare(panel, waveData, npoints);
				}
				else
				{
					filtrare_plotare(panel, secunde[nextsec - 1], npunctepersec);
				}
			}
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a ring-ului Filtru 
	0 -> filtrare prin mediere 
	1 -> filtrare de ordin intai
*******************************************************************************/
int CVICALLBACK On_Filtru (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a butonului Prev
*******************************************************************************/
int CVICALLBACK On_Previous (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	double *currentsec = 0;
	int maxIdx, minIdx;
	double medie = 0.0;
	double dispersie = 0.0;
	double mediana = 0.0;
	double zero_crossing = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			if(nextsec == 1)
			{
				//afisare pe grapf
				DeleteGraphPlot(panel, PANEL_GRAPH_RAW_DATA, -1, VAL_IMMEDIATE_DRAW );
				PlotY(panel, PANEL_GRAPH_RAW_DATA, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
				
				//minim
				MaxMin1D(waveData, npoints, &maxVal, &maxIdx, &minVal, &minIdx);
				SetCtrlVal(panel, PANEL_MINIM, minVal);
				SetCtrlVal(panel, PANEL_INDEX_MINIM, minIdx);
				
				//maxim
				MaxMin1D(waveData, npoints, &maxVal, &maxIdx, &minVal, &minIdx);
				SetCtrlVal(panel, PANEL_MAXIM, minVal);
				SetCtrlVal(panel, PANEL_INDEX_MAXIM, maxIdx);
				
				//medie + dispersie
				Variance(waveData, npoints, &medie, &dispersie);
				SetCtrlVal(panel, PANEL_MEDIE, medie);
				SetCtrlVal(panel, PANEL_DISPERSIE, sqrt(dispersie));
				
				//mediana
				Median(waveData, npoints, &mediana);
				//printf("%lf", mediana);
				SetCtrlVal(panel, PANEL_MEDIANA, mediana);
				
				//zero_crossing
				for(int i = 0; i < npoints - 1; ++i)
				{
					if((waveData[i] < 0 && waveData[i+1] > 0) || (waveData[i] > 0 && waveData[i+1] < 0))
					{
						zero_crossing++;
					}
				}
				SetCtrlVal(panel, PANEL_ZERO_CROSSING, zero_crossing);
				
				if(am_apasat_anvelopa == 1)
				{
					//DeleteGraphPlot(panel, PANEL_GRAPH_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
					//PlotY(panel, PANEL_GRAPH_RAW_DATA, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
					//printf("%d ", nextsec);
					PlotY(panel, PANEL_GRAPH_RAW_DATA, anvelopa, npoints, VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_DK_BLUE);
					
				}
				if(am_apasat_aplica == 1)
				{
					filtrare_plotare(panel, waveData, npoints);
				}
				
				nextsec --;
				SetCtrlVal(panel, PANEL_STOP, nextsec);
			}
			if(nextsec > 1 && am_apasat_load == 1)
			{
				nextsec --;
				
				currentsec = secunde[nextsec - 1];
				
				DeleteGraphPlot(panel, PANEL_GRAPH_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, PANEL_GRAPH_RAW_DATA, currentsec, npunctepersec, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);

				//minim
				MaxMin1D(currentsec, npunctepersec, &maxVal, &maxIdx, &minVal, &minIdx);
				SetCtrlVal(panel, PANEL_MINIM, minVal);
				SetCtrlVal(panel, PANEL_INDEX_MINIM, minIdx);
				
				//maxim
				MaxMin1D(currentsec, npunctepersec, &maxVal, &maxIdx, &minVal, &minIdx);
				SetCtrlVal(panel, PANEL_MAXIM, minVal);
				SetCtrlVal(panel, PANEL_INDEX_MAXIM, maxIdx);
				
				//medie + dispersie
				Variance(currentsec, npunctepersec, &medie, &dispersie);
				SetCtrlVal(panel, PANEL_MEDIE, medie);
				SetCtrlVal(panel, PANEL_DISPERSIE, sqrt(dispersie));
				
				//mediana
				Median(currentsec, npunctepersec, &mediana);
				//printf("%lf", mediana);
				SetCtrlVal(panel, PANEL_MEDIANA, mediana);
				
				//zero_crossing
				for(int i = 0; i < npunctepersec - 1; ++i)
				{
					if((currentsec[i] < 0 && currentsec[i+1] > 0) || (currentsec[i] > 0 && currentsec[i+1] < 0))
					{
						zero_crossing++;
					}
				}
				SetCtrlVal(panel, PANEL_ZERO_CROSSING, zero_crossing);
				
				if(am_apasat_anvelopa == 1)
				{
					//DeleteGraphPlot(panel, PANEL_GRAPH_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
					//PlotY(panel, PANEL_GRAPH_RAW_DATA, currentsec, npunctepersec, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
					//printf("%d ", nextsec);
					PlotY(panel, PANEL_GRAPH_RAW_DATA, anvelopapersec[nextsec - 1], npunctepersec, VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_DK_BLUE);
					
				}
				
				SetCtrlVal(panel, PANEL_STOP, nextsec);
				
				if(am_apasat_aplica == 1)
				{
					filtrare_plotare(panel, currentsec, npunctepersec);
				}
				
				
			}
			
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a butonului Next
*******************************************************************************/
int CVICALLBACK On_Next (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	double *currentsec = 0;
	int maxIdx, minIdx;
	double medie = 0.0;
	double dispersie = 0.0;
	double mediana = 0.0;
	double zero_crossing = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			
			if(nextsec <= 5 && am_apasat_load == 1)
			{
				currentsec = secunde[nextsec];
				
				DeleteGraphPlot(panel, PANEL_GRAPH_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, PANEL_GRAPH_RAW_DATA, currentsec, npunctepersec, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);

				//minim
				MaxMin1D(currentsec, npunctepersec, &maxVal, &maxIdx, &minVal, &minIdx);
				SetCtrlVal(panel, PANEL_MINIM, minVal);
				SetCtrlVal(panel, PANEL_INDEX_MINIM, minIdx);
				
				//maxim
				MaxMin1D(currentsec, npunctepersec, &maxVal, &maxIdx, &minVal, &minIdx);
				SetCtrlVal(panel, PANEL_MAXIM, minVal);
				SetCtrlVal(panel, PANEL_INDEX_MAXIM, maxIdx);
				
				//medie + dispersie
				Variance(currentsec, npunctepersec, &medie, &dispersie);
				SetCtrlVal(panel, PANEL_MEDIE, medie);
				SetCtrlVal(panel, PANEL_DISPERSIE, sqrt(dispersie));
				
				//mediana
				Median(currentsec, npunctepersec, &mediana);
				//printf("%lf", mediana);
				SetCtrlVal(panel, PANEL_MEDIANA, mediana);
				
				//zero_crossing
				for(int i = 0; i < npunctepersec - 1; ++i)
				{
					if((currentsec[i] < 0 && currentsec[i+1] > 0) || (currentsec[i] > 0 && currentsec[i+1] < 0))
					{
						zero_crossing++;
					}
				}
				SetCtrlVal(panel, PANEL_ZERO_CROSSING, zero_crossing);
				
				
				if(am_apasat_aplica == 1)
				{
					filtrare_plotare(panel, currentsec, npunctepersec);
				}
				
				if(am_apasat_anvelopa == 1)
				{
					//DeleteGraphPlot(panel, PANEL_GRAPH_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
					//PlotY(panel, PANEL_GRAPH_RAW_DATA, currentsec, npunctepersec, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
					//printf("%d ", nextsec);
					PlotY(panel, PANEL_GRAPH_RAW_DATA, anvelopapersec[nextsec], npunctepersec, VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_DK_BLUE);
				}
				
				nextsec ++;
				
				SetCtrlVal(panel, PANEL_STOP, nextsec);
			}
				
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a butonului Anvelopa
*******************************************************************************/
int CVICALLBACK On_Avelopa (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//panelAnvelopa = LoadPanel(1, "lucru.uir", PANEL_ANVE);
			//InstallPopup(panelAnvelopa);
			
			//DeleteGraphPlot(panelAnvelopa, PANEL_ANVE_GRAPH_ANVELOPA, -1, VAL_IMMEDIATE_DRAW);
			am_apasat_anvelopa = 1;
			
			anvelopa = (double*)malloc(sizeof(double)*npoints);
			
			FileToArray("anvelopadata.txt", anvelopa, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_ASCII);
			
			anvelopapersec = (double**) malloc (sizeof(double*)*intervalesec);
			for (int i = 0; i < intervalesec; ++i) 
			{
        		anvelopapersec[i] = (double*) malloc(sizeof (double) * npunctepersec); //alocare memorie coloane
    		}
			
			//construirea vectorului cu secunde
			int count = 0; //numar de coloana -> adica in a cata coloana sunt
			for(int i = 0; i < intervalesec; ++i) //parcurg liniile -> adica fiecare secunda 0->1; 1->2 etc. 
			{
				for(int j = npunctepersec * i; j < npunctepersec * (i + 1); ++j) // parcurg vectorul initial(waveData) pentru a lua secundele din el
				{
					anvelopapersec[i][count] = anvelopa[j];
					count++;
				}
				count = 0;
			}
			
			/*
			for(int i = 0; i < intervalesec; ++i)
			{
				for(int count = 0; count < npunctepersec; count++)
				{
					printf("%lf ", anvelopapersec[i][count]);
				}
				
			}
			*/
			//PlotY(panel, PANEL_GRAPH_RAW_DATA, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_MAGENTA); //semnalul waveData
			PlotY(panel, PANEL_GRAPH_RAW_DATA, anvelopa, npoints, VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_DK_BLUE); //anvelopa
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a panoului cu anvelopa 
    !Acesta nu va mai fi folosit caci voi afisa anvelopa pe graficul Raw Data!
*******************************************************************************/
/*int CVICALLBACK OnPanelAnvelopa (int panel, int event, void *callbackData,
								 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			RemovePopup(panelAnvelopa);
			break;
	}
	return 0;
}*/

/*******************************************************************************
	Functia de callback a butonului Derivata
*******************************************************************************/
int CVICALLBACK On_Derivata (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			panelDerivata = LoadPanel(1, "lucru.uir", PANEL_DERI);
			InstallPopup(panelDerivata);
			
			double* derivata = (double *) malloc (sizeof(double)*npoints);
			DifferenceEx(waveData, npoints, 1.0, waveData + 1, 1, waveData + npoints - 1, 1, DIFF_SECOND_ORDER_CENTRAL, derivata);
			
			PlotY(panelDerivata, PANEL_DERI_GRAPH_DERIVATA, derivata, npoints, VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_DK_CYAN);
			
			
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a panoului cu derivata
*******************************************************************************/
int CVICALLBACK OnPanelDerivata (int panel, int event, void *callbackData,
								 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			RemovePopup(panelDerivata);
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a butonului Save
*******************************************************************************/
int CVICALLBACK On_Save (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	int val;
	switch (event)
	{
		case EVENT_COMMIT:
			int imghandle ;
			char fileName[100] = {0};
			
			//fac imaginea pentru semnalul initial
			if(nextsec == 0)
			{ 
				//semnalul initial
				sprintf(fileName,"Imagine_semnal_initial.jpg");
			}
			else
			{ 
				//pe secunda
				sprintf(fileName,"Imagine_semnal_initial_secunda_%d.jpg", nextsec - 1);
			}
			
			GetCtrlDisplayBitmap(panel, PANEL_GRAPH_RAW_DATA, 1, &imghandle);
			//daca la fileName nu ii dam calea, el se va salva in folderul proiectului
			SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
			
			GetCtrlVal(panel, PANEL_FILTRU, &val);
			if(val == 0) // asa fac diferentiere intre filtre
			{
				//fac imaginea pentru semnalul initial
				if(nextsec == 0)
				{ 
					//filtru mediere
					sprintf(fileName,"Imagine_semnal_filtrat_mediere.jpg");
				}
				else 
				{ 
					//pe secunda
					sprintf(fileName,"Imagine_semnal_filtrat_mediere_secunda_%d.jpg", nextsec - 1);
				}
			}else if(val == 1)
			{
				//fac imaginea pentru semnalul initial
				if(nextsec == 0)
				{ 
					//filtru alpha
					sprintf(fileName,"Imagine_semnal_filtrat_alpha.jpg");
				}
				else 
				{ 
					//pe secunda
					sprintf(fileName,"Imagine_semnal_filtrat_alpha_secunda_%d.jpg", nextsec - 1);
				}
			}
			GetCtrlDisplayBitmap(panel, PANEL_GRAPH_FILTRED_DATA, 1, &imghandle);
			//daca la fileName nu ii dam calea, el se va salva in folderul proiectului
			SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
			DiscardBitmap(imghandle);
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback de tranzitie intre panouri
*******************************************************************************/
int CVICALLBACK OnSwitchWaveFreq (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel == panelHandle)
			{
				SetCtrlVal(panelFrecventa, PANEL_FREQ_SWITCH_WAVE_FREQ, 1);
				DisplayPanel(panelFrecventa);
				HidePanel(panel);
			}
			else
			{
				SetCtrlVal(panelHandle, PANEL_SWITCH_WAVE_FREQ, 0);
				DisplayPanel(panelHandle);
				HidePanel(panel);
			}
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a celui de al doilea panou
*******************************************************************************/
int CVICALLBACK OnPanelFreq (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a butonului Spectru
	!Acesta nu va mai fi folosit caci se va afisa direct spectrul pe grafice 
	prin apelarea functiei create!
*******************************************************************************/
int CVICALLBACK OnSpectru (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

/*******************************************************************************
	Functia pentru generarea spectrului
*******************************************************************************/
void Spectru(int numRead, double *vect, int index)
{
        //vector ce contine spectrul semnalului convertit la volti
   		double convertedSpectrum[numRead/2];
        //frecventa estimata pentru spectrul de putere (maxim) din vectorul autoSpectrum
    	double powerPeak = 0.0;
        //valoarea maxima din spectru de putere (din autoSpectrum)
    	double freqPeak = 0.0;
        //variabila ce reprezinta pasul in domeniul frecventei
    	double df = 0.0;
        //voltage signal - descriere a semnalului achizitionat
    	char unit[32] = "V";
        //spectrul de putere cu un numar de valori egal cu jumatate din dimensiunea bufferuluide intrare
    	double autoSpectrum[numRead/2];
        //
    	WindowConst winConst;
    
        //aplatizare forma semnal la capetele intervalului
		int valfereastra;
		GetCtrlVal(panelFrecventa, PANEL_FREQ_SWITCH_WINDOW_TYPE, &valfereastra);
		if(valfereastra == 0)
		{
			ScaledWindowEx (vect, numRead, 5, -1, &winConst);
		}
		else
		{
			ScaledWindowEx (vect, numRead, 30, -1, &winConst);
		}
        
    
        //partea pozitiva a spectrului scalat de putere pentru un semnal esantionat
        AutoPowerSpectrum(vect, numRead, 1.0/frecventa, autoSpectrum, &df);

        //calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului  
        PowerFrequencyEstimate (autoSpectrum, numRead/2, -1, winConst, df, 7, &freqPeak, &powerPeak);
		if(index == 1)
		{	
			SetCtrlVal(panelFrecventa, PANEL_FREQ_FREQUENCY_PEAK, freqPeak);
			SetCtrlVal(panelFrecventa, PANEL_FREQ_POWER_PEAK, powerPeak);
		}
		else
		{
			SetCtrlVal(panelFrecventa, PANEL_FREQ_FREQUENCY_PEAK_2, freqPeak);
			SetCtrlVal(panelFrecventa, PANEL_FREQ_POWER_PEAK_2, powerPeak);
		}
		SpectrumUnitConversion(autoSpectrum, numRead/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrum, unit);
		if(index == 1)
		{
			DeleteGraphPlot(panelFrecventa, PANEL_FREQ_GRAPH_SPECTRU_1, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform(panelFrecventa, PANEL_FREQ_GRAPH_SPECTRU_1,convertedSpectrum,numRead/2,VAL_DOUBLE,1.0,0.0,0.0,df,VAL_THIN_LINE,VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
		}
		else
		{
			DeleteGraphPlot(panelFrecventa, PANEL_FREQ_GRAPH_SPECTRU_2, -1, VAL_IMMEDIATE_DRAW);	
			PlotWaveform(panelFrecventa, PANEL_FREQ_GRAPH_SPECTRU_2,convertedSpectrum,numRead/2,VAL_DOUBLE,1.0,0.0,0.0,df,VAL_THIN_LINE,VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
		}
}

/*******************************************************************************
	Functia de callback a timer-ului
*******************************************************************************/
int parcurgesecunda = 0; 

int CVICALLBACK OnTimer (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	int imghandle ;
	char fileName[100] = {0};
	int val;
	int valfereastra, valfiltru, valsecunda;
	switch (event)
	{
		case EVENT_TIMER_TICK:
			GetCtrlVal(panel, PANEL_FREQ_DIM_FEREASTRA, &val);
			int punctefereastra = pow(2, val);
			GetCtrlVal(panel, PANEL_FREQ_SWITCH_WINDOW_TYPE, &valfereastra);
			GetCtrlVal(panel, PANEL_FREQ_SWITCH_FILTER_TYPE, &valfiltru);
			GetCtrlVal(panel, PANEL_FREQ_SECUNDA, &valsecunda);
			
			double * Fereastra = (double *) malloc (sizeof(double) * punctefereastra); 

			//preluarea secventei din secunda
			if((parcurgesecunda + punctefereastra) < npunctepersec)
			{
				Fereastra = secunde[valsecunda] + parcurgesecunda;

				DeleteGraphPlot(panel, PANEL_FREQ_GRAPH_RAW_SIGNAL_2, -1, VAL_IMMEDIATE_DRAW );
				PlotY(panel, PANEL_FREQ_GRAPH_RAW_SIGNAL_2, Fereastra , punctefereastra, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_MAGENTA);

			}
			else
			{ 
				//daca nu imi incap punctele din semnal fac ZERO PADDING
				int count = parcurgesecunda;
				for(int i = 0; i < punctefereastra ; ++i)
				{
					if(count < npunctepersec)
						Fereastra[i] = secunde[valsecunda][i];  
					else
						Fereastra[i] = 0;
					count++;
				}
			
			DeleteGraphPlot(panel, PANEL_FREQ_GRAPH_RAW_SIGNAL_2, -1, VAL_IMMEDIATE_DRAW );
			PlotY(panel, PANEL_FREQ_GRAPH_RAW_SIGNAL_2, Fereastra , punctefereastra, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_MAGENTA);

			//se opreste pentru ca a ajuns la finalul secundei
			SetCtrlAttribute(panel, PANEL_FREQ_TIMER, ATTR_ENABLED, 0);
			SetCtrlAttribute(panel, PANEL_FREQ_TIMERBUTTON, ATTR_CTRL_VAL,  0);
			//parcurgesecunda = 0;
			}
			
			//FERESTRUIRE + SPECTRU FERESTRUIRE
			double * vectFerestruit = (double *) malloc (sizeof(double) * punctefereastra);
			
			if(valfereastra == 0)
			{
				//BlackMan
				Copy1D(Fereastra, punctefereastra, vectFerestruit);
				BkmanWin(vectFerestruit, punctefereastra);
				DeleteGraphPlot(panel, PANEL_FREQ_GRAPH_FERESTRUIRE, -1, VAL_IMMEDIATE_DRAW );
				PlotY(panel, PANEL_FREQ_GRAPH_FERESTRUIRE, vectFerestruit , punctefereastra, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
				Spectru(punctefereastra, vectFerestruit, 1);
				
				//Salvare
				sprintf(fileName,"Imagine_semnal_ferestruit_blackman_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
				GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_FERESTRUIRE, 1, &imghandle);
				SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
				//Salvare
				sprintf(fileName,"Spectru_ferestruit_bkman_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
				GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_SPECTRU_1, 1, &imghandle);
				SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
			}
			else
			{
				//Triunghiular
				Copy1D(Fereastra, punctefereastra, vectFerestruit);
				TriWin(vectFerestruit, punctefereastra);
				DeleteGraphPlot(panel, PANEL_FREQ_GRAPH_FERESTRUIRE, -1, VAL_IMMEDIATE_DRAW );
				PlotY(panel, PANEL_FREQ_GRAPH_FERESTRUIRE, vectFerestruit , punctefereastra, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
				Spectru(punctefereastra, vectFerestruit, 1);
				
				//Salvare
				sprintf(fileName,"Imagine_semnal_ferestruit_triunghiular_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
				GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_FERESTRUIRE, 1, &imghandle);
				SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
				//Salvare
				sprintf(fileName,"Spectru_ferestruit_tri_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
				GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_SPECTRU_1, 1, &imghandle);
				SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
			}
			
			
			//FILTRU + FEREASTRA + SPECTRU
			double * Filtru = (double *) malloc (sizeof(double) * punctefereastra);
			
			if(valfiltru == 0)
			{
				//Eliptic
				double ripple;
				double atenuare;
				int ordin;
				GetCtrlVal(panel, PANEL_FREQ_RIPPLE, &ripple);
				GetCtrlVal(panel, PANEL_FREQ_ATENUARE, &atenuare);
				GetCtrlVal(panel, PANEL_FREQ_ORDIN, &ordin);
				Elp_LPF(Fereastra, punctefereastra, frecventa, 1500, ripple, atenuare, ordin, Filtru);
				
					if(valfereastra == 0)
					{
						//BlackMan
						//Copy1D(Fereastra, punctefereastra, vectFerestruit);
						BkmanWin(Filtru, punctefereastra);
						DeleteGraphPlot(panel, PANEL_FREQ_GRAPH_FILTRU, -1, VAL_IMMEDIATE_DRAW );
						PlotY(panel, PANEL_FREQ_GRAPH_FILTRU, Filtru , punctefereastra, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
						Spectru(punctefereastra, Filtru, 0);
						
						//Salvare
						sprintf(fileName,"Imag_filtrat_elp_ord_%d_ferestruit_bkman_secunda_%d_%d-%d.jpg", ordin, valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
						GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_FILTRU, 1, &imghandle);
						SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
						//Salvare
						sprintf(fileName,"Spectru_filtrat_elp_ord_%d_ferestruit_bkman_secunda_%d_%d-%d.jpg", ordin, valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
						GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_SPECTRU_2, 1, &imghandle);
						SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
					}
					else
					{
						//Triunghiular
						//Copy1D(Fereastra, punctefereastra, vectFerestruit);
						TriWin(Filtru, punctefereastra);
						DeleteGraphPlot(panel, PANEL_FREQ_GRAPH_FILTRU, -1, VAL_IMMEDIATE_DRAW );
						PlotY(panel, PANEL_FREQ_GRAPH_FILTRU, Filtru , punctefereastra, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
						Spectru(punctefereastra, Filtru, 0);
						
						//Salvare
						sprintf(fileName,"Imag_filtrat_elp_ferestruit_tri_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
						GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_FILTRU, 1, &imghandle);
						SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
						//Salvare
						sprintf(fileName,"Spectru_filtrat_elp_ferestruit_tri_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
						GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_SPECTRU_2, 1, &imghandle);
						SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
					}
			
			}
			else
			{
				//Notch
				int length, m;
				int impulse_length = punctefereastra;
				double ripple;
				double atenuare;
				double delta, freq_stopband = 950.0, freq_passband = 1050.0; 
	 
				GetCtrlVal(panel, PANEL_FREQ_RIPPLE, &ripple);
				GetCtrlVal(panel, PANEL_FREQ_ATENUARE, &atenuare);
				int numtaps_FIR;
				double rs = pow(10, - atenuare/20.0);
	 
				//double * vectFiltru =(double *) malloc(sizeof (double) * punctefereastra);
	 
				//FIIR
				delta = (freq_passband - freq_stopband) * 0.5 / frecventa;
				numtaps_FIR = (int)((-20.0 * log( sqrt(ripple * rs) ) - 13) / (14.6 * delta) * 1.1);
				FIRCoefStruct structFir;
				FIRNarrowBandCoef(frecventa, freq_passband, freq_stopband, 1000.0, ripple, atenuare,  BANDSTOP__, &structFir);
 
				if(structFir.interp == 1)
				{
					length = impulse_length + structFir.Mtaps - 1;
				}
				else
				{
					length = impulse_length + (structFir.Mtaps - 1) * structFir.interp + structFir.Itaps - 1;
				}
 
				m = punctefereastra + (structFir.Mtaps - 1) * structFir.interp + structFir.Itaps - 1;
				double *vectFil = (double *) malloc(m * sizeof(double));
				
				FIRNarrowBandFilter(Fereastra, punctefereastra, structFir, vectFil);
				if(valfereastra == 1)
				{
 					//Blackman
					BkmanWin(vectFil, punctefereastra);
					DeleteGraphPlot(panel, PANEL_FREQ_GRAPH_FILTRU, -1, VAL_IMMEDIATE_DRAW );
					PlotY(panel, PANEL_FREQ_GRAPH_FILTRU, vectFil , punctefereastra, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
					Spectru(punctefereastra, vectFil, 0);
 
					//Salvare
					sprintf(fileName,"Imag_filtrat_notch_ferestruit_bkman_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
					GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_FILTRU, 1, &imghandle);
					SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
					//Salvare
					sprintf(fileName,"Spectru_filtrat_notch_ferestruit_bkman_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
					GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_SPECTRU_2, 1, &imghandle);
					SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
				} 
				else
				{
					//Triunghiular
					TriWin(vectFil, punctefereastra);
					DeleteGraphPlot(panel, PANEL_FREQ_GRAPH_FILTRU, -1, VAL_IMMEDIATE_DRAW );
					PlotY(panel, PANEL_FREQ_GRAPH_FILTRU, vectFil , punctefereastra, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
					Spectru(punctefereastra, vectFil, 0);
					
					//Salvare
					sprintf(fileName,"Imag_filtrat_notch_ferestruit_tri_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
					GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_FILTRU, 1, &imghandle);
					SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
					//Salvare
					sprintf(fileName,"Spectru_filtrat_notch_ferestruit_tri_secunda_%d_%d-%d.jpg", valsecunda, parcurgesecunda, parcurgesecunda + punctefereastra);
					GetCtrlDisplayBitmap(panel, PANEL_FREQ_GRAPH_SPECTRU_2, 1, &imghandle);
					SaveBitmapToJPEGFile(imghandle, fileName, JPEG_PROGRESSIVE, 100);
			}
			}
			
			//printf("%d, ", parcurgesecunda);
			parcurgesecunda += punctefereastra;
			//printf("%d\n", parcurgesecunda);
			break;
	}
	return 0;
}

/*******************************************************************************
	Functia de callback a butonului de activare a timer-ului
*******************************************************************************/
int CVICALLBACK OnActivareTimer (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	int val;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, PANEL_FREQ_TIMERBUTTON, &val);
			if(val == 1)
			{
				SetCtrlAttribute(panel, PANEL_FREQ_TIMER, ATTR_ENABLED, val);
				parcurgesecunda = 0;
			}
			else
			{
				SetCtrlAttribute(panel, PANEL_FREQ_TIMER, ATTR_ENABLED, val);
			}
			
			break;
	}
	return 0;
}