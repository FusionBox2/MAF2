//******************************************
//definizione codici errori
//******************************************
#define NOERROR								 0

#define ERROR_OPEN_FILE						 1
#define ERROR_READING_PROC_TYPE				 2
#define ERROR_READING_HEADER				 3
#define ERROR_READING_PARAM					 4
#define ERROR_READING_TRIAL_PARAM			 5
#define ERROR_READING_VIDEO_PARAM			 6
#define ERROR_READING_ANALOG_PARAM			 7
#define ERROR_READING_FORCE_PLATFORM_PARAM	 8
#define ERROR_READING_EVENT_PARAM			 9
#define ERROR_READING_DATA					10
#define ERROR_READING_VIDEO_DATA			11
#define ERROR_READING_ANALOG_DATA			12
#define ERROR_CLOSE_FILE					13

#define ERROR_NOT_LICENSE					100

//******************************************
//definizione costanti
//******************************************
#define TRAJECTORY				1
#define ANGLE					2
#define MOMENT					3
#define POWER				    4

//******************************************
//funzioni importate
//******************************************
__declspec(dllimport) 	int getNumTraj(void);								//restituisce il numero di traiettorie
__declspec(dllimport) 	int getTotalFrameTraj(void);						//restituisce il numero di frame totali
__declspec(dllimport) 	char* getNameTraj(int indexTraj);					//restituisce il nome della traiettoria dato l'indice indexTraj
__declspec(dllimport) 	char* getUnitTraj(int indexTraj);					//restituisce l'unità di misura delle traiettorie dato l'indice indexTraj
__declspec(dllimport) 	int getTypeTraj(int indexTraj);						//restituisce il tipo di traiettoria(traiettoria, angolo, momento, potenza) dato l'indice indexTraj
__declspec(dllimport) 	double getXTraj(int indexTraj, int indexFrame);		//restituisce la componente x della traiettoria dato l'indice indexTraj e il frame indexFrame
__declspec(dllimport) 	double getYTraj(int indexTraj, int indexFrame);		//restituisce la componente y della traiettoria dato l'indice indexTraj e il frame indexFrame
__declspec(dllimport) 	double getZTraj(int indexTraj, int indexFrame);		//restituisce la componente z della traiettoria dato l'indice indexTraj e il frame indexFrame
__declspec(dllimport) 	bool isDefinedTraj(int indexTraj, int indexFrame);	//restituisce "true" se la traiettoria esiste dato l'indice indexTraj e il frame indexFrame, "false" altrimenti
	
__declspec(dllimport) 	int getChannelsAnalog(void);							//restituisce il numero di canali
__declspec(dllimport) 	int getTotalSamplesAnalog(void);						//restituisce il numero di campioni totali
__declspec(dllimport) 	char* getNameAnalog(int indexChan);						//restituisce il nome del canale dato l'indice indexChan
__declspec(dllimport) 	char* getUnitAnalog(int indexChan);						//restituisce l'unità di misura del canali dato l'indice indexChan
__declspec(dllimport) 	double getValueAnalog(int indexChan, int indexSample);	//restituisce il valore del segnale analogico dato l'indice indexChan e il campione indexSample

__declspec(dllimport) 	int getPlatforms(void);																		//restituisce il numero di piattaforme
__declspec(dllimport) 	void getCornerPlatform(int indexPlatform, int indexCorner, double *coordX, double *coordY);	//fornisce le coordinate del corner indexCorner della piattaforma indexPlatform
__declspec(dllimport) 	void getCenterPlatform(int indexPlatform, double *centerX, double *centerY);				//fornisce le coordinate del centro geometrico della piattaforma indexPlatform
__declspec(dllimport) 	double getCOPX(int indexPlatform, int indexSample);											//restituisce la coordinata X del COP data la piattaforma indexPlatform e il campione indexSample
__declspec(dllimport) 	double getCOPY(int indexPlatform, int indexSample);											//restituisce la coordinata Y del COP data la piattaforma indexPlatform e il campione indexSample
__declspec(dllimport) 	double getFx(int indexPlatform, int indexSample);											//restituisce la componente X della forza data la piattaforma indexPlatform e il campione indexSample
__declspec(dllimport) 	double getFy(int indexPlatform, int indexSample);											//restituisce la componente Y della forza data la piattaforma indexPlatform e il campione indexSample
__declspec(dllimport) 	double getFz(int indexPlatform, int indexSample);											//restituisce la componente Z della forza data la piattaforma indexPlatform e il campione indexSample
__declspec(dllimport) 	double getMx(int indexPlatform, int indexSample);											//restituisce la componente X del momento data la piattaforma indexPlatform e il campione indexSample
__declspec(dllimport) 	double getMy(int indexPlatform, int indexSample);											//restituisce la componente Y del momento data la piattaforma indexPlatform e il campione indexSample
__declspec(dllimport) 	double getMz(int indexPlatform, int indexSample);											//restituisce la componente Z del momento data la piattaforma indexPlatform e il campione indexSample

__declspec(dllimport) 	int getEvents(void);						//restituisce il numero degli eventi
__declspec(dllimport) 	char* getContextEvent(int indexEvent);		//restituisce il contesto dell'evento dato l'indice indexEvent 
__declspec(dllimport) 	double getValueEvent(int indexEvent);		//restituisce il valore dell'evento (in secondi) dato l'indice indexEvent

//------------------------------------------------------------------
//C3D_Open: Funzione di apertura file
//INPUT:	nome file con intero percorso
//OUTPUT:	NOERROR se aperto correttamente 
//			ERROR_NOT_LICENSE in caso di licenza errata o mancante
//			ERROR_OPEN_FILE in caso di errore nell'apertura del file
//			ERROR_READING_PROC_TYPE in caso di errore nella lettura del tipo (PC, DEC, MIPS)
//------------------------------------------------------------------
__declspec(dllimport) int C3D_Open(char* fileName);

//------------------------------------------------------------------
//C3D_Read_Header: Funzione di lettura header
//OUTPUT:	NOERROR se legge correttamente l'header e restituisce:
//					- lunghezza trial in ms
//					- frequenza di acquisizione cinematica
//					- frequenza di acquisizione analogica
//			ERROR_READING_HEADER in caso di errore
//			ERROR_READING_PARAM in caso di errore nella lettura dei parametri generali
//			ERROR_READING_TRIAL_PARAM in caso di errore nella lettura dei parametri della sezione trial
//------------------------------------------------------------------
__declspec(dllimport) int C3D_Read_Header(long *lengthMs, float *videoRate, float *analogRate);

//------------------------------------------------------------------
//Funzione di lettura dati:sezione cinematica, segnali analogici, eventi
//OUTPUT:	NOERROR se legge correttamente la sezione dati
//			ERROR_READING_PARAM in caso di errore nella lettura dei parametri generali
//			ERROR_READING_TRIAL_PARAM in caso di errore nella lettura dei parametri della sezione trial
//			ERROR_READING_VIDEO_PARAM in caso di errore nella lettura dei parametri della sezione cinematica
//			ERROR_READING_ANALOG_PARAM in caso di errore nella lettura dei parametri della sezione analogica
//			ERROR_READING_FORCE_PLATFORM_PARAM in caso di errore nella lettura dei parametri della sezione piattaforme di forza
//			ERROR_READING_EVENT_PARAM in caso di errore nella lettura dei parametri della sezione eventi
//			ERROR_READING_DATA in caso di errore nella lettura dei dati
//			ERROR_READING_VIDEO_DATA in caso di errore nella lettura dei dati cinematici
//			ERROR_READING_ANALOG_DATA in caso di errore nella lettura dei dati analogici
//------------------------------------------------------------------
__declspec(dllimport) int C3D_Read_Data(void);

//------------------------------------------------------------------
//Funzione di calcolo dati: forze provenienti dalle piattaforme
//OUTPUT:	NOERROR se calcola correttamente i dati
//------------------------------------------------------------------
__declspec(dllimport) int C3D_Calculate_Data(void);

//------------------------------------------------------------------
//Funzione di chiusura file
//INPUT:	-
//OUTPUT:	NOERROR se chiuso correttamente, 
//			ERROR_CLOSE_FILE in caso di errore nella chiusura del file
//------------------------------------------------------------------
__declspec(dllimport) int C3D_Close(void);
