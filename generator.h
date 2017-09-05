#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>
#include <generators_global.h>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <QVector>
#include <QTimerEvent>
#include <calibrator.h>
#include <server.h>
#include <cmath>


typedef int FrequencyGrid;
typedef int LevelControlMode;
typedef int FmMode;


/* Класс Generator является базовым для всех USB генераторов компании РАДИЙ ТН и
 * и определяет интерфейс производных классов
 *
* Все велечины передаются в системе Cи ( Гц, В, сек).
* Стратегия обработки ошибок: при возникновении ошибки при управлении генератором
* излучается соответстующий сигнал. Дополнительная информация о работе возможна при запуске через
* консоль, если ее разрешить вызовом метода enableVerbose(true). Так же вся информация сохраняется
* в файле log.txt, которые находится в одной папке с испольняемым файлом
*/

class GENERATORS_EXPORT Generator : public QObject
{
  Q_OBJECT
public:
    explicit Generator(int i_vid, int i_pid, float i_lowestFreq, float i_highestFreq, float i_tFmMin, float i_tFmMax, float i_fFmBandStop, QObject * parent = 0);

public slots:
    void errorSlot(QString err);
    //Включение генератора
    virtual bool GENERATORS_EXPORT turnOn(bool i_on) = 0;

    // Установка амплитуды
    virtual bool GENERATORS_EXPORT setAmp(float &m_amp) = 0;

    // Установка частоты
    virtual bool GENERATORS_EXPORT setFrequency(float &m_f) = 0;

    // Запуск ЧМ
    bool GENERATORS_EXPORT startFm(float &m_fStart, float &m_fStop, float &m_fStep, float &m_timeStep);
    // Остановка ЧМ
    void GENERATORS_EXPORT stopFm();
    // Выбор режима ЧМ
    void GENERATORS_EXPORT setFmMode(FmMode mode);

public:
    // Возврат текущего значения амплитуды
    float GENERATORS_EXPORT getAmp();

    // Возврат текущего значения частоты
    float GENERATORS_EXPORT getFrequency();

    // Установка шага частотной сетки
    virtual void GENERATORS_EXPORT setFrequencyGrid(int i_frequencyGrid) = 0;
    // Возврат шага частотной сетки
    virtual FrequencyGrid GENERATORS_EXPORT getFrequencyGrid() = 0;


    //Установить связь с заданным устройством
    virtual bool GENERATORS_EXPORT connect(QSerialPortInfo &info);
    // Закрыть устройство
    void GENERATORS_EXPORT disconnect() ;

    // Получить список доступных устройств
    static QList<QSerialPortInfo> GENERATORS_EXPORT getAvailablePorts();
    // Получить информацию о текущем порте
    QSerialPortInfo GENERATORS_EXPORT getPortInfo();

    // Включить вывод информации в консоль
    void GENERATORS_EXPORT enableVerbose(bool);
    // Включить логи
    void GENERATORS_EXPORT enableLogs(bool);

    // Переключение режима управление сигнала (по умолчанию стоит режим управления амплитудой)
    virtual void GENERATORS_EXPORT setLevelControlMode(LevelControlMode mode) = 0;
    // Возврат текущего режима управления сигналом генератора
    virtual LevelControlMode GENERATORS_EXPORT getLevelControlMode() = 0;

    int GENERATORS_EXPORT getPid();
    int GENERATORS_EXPORT getVid();

    //Установка номера порта, по которому генератор будет слушать входящие комадны.
    void GENERATORS_EXPORT setTcpPort(int );
    int GENERATORS_EXPORT getTcpPort() const;
    QHostAddress GENERATORS_EXPORT getIpAddress() const;

    //возможные сетки частот генератора
    enum eFrequencyGrid {
        Grid1, // 1 Кгц
        Grid2, // 2 КГц
        Grid5, // 5 КГц
        Grid10 // 10 КГц
    };

    // режимы качания частоты
    enum eFmMode{
        UpChirp, // от меньшей к большей
        DownChirp,   // от большей к меньшей
        FHSS // ППРЧ
    };

    // режимы управления уровнем сигнала генератора
    enum eLevelControlMode{
        Amplitude,
        Attenuation
    };



signals:
    void error(QString e);
    void disconnected();
    void newFrequency(float freq_Hz);
    void newAmplitude(float amp_V);
    void newState(bool on);
    void netControl(bool on);
    void turnedOn(bool on);
    void newTFm(float t_s);



protected slots:
    void  printMessage(QString message);
    void amplitudeChanged(float amp);
    void frequencyChanged(float freq);
    void stateChanged(bool on);
    void serverConnected();
    void serverDisconnected();

protected:

    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    void fmIteration();

    float roundToGrid(float);
    double log2(double x);

    Calibrator calibrator;
    Server server;

    static int objectCounter;
    const int vid;
    const int pid;

    bool on;
    bool connected;

    bool verbose;
    bool logs;

    const float lowestFrequency;
    const float highestFrequency;
    int frequencyGrid;
    float currentFrequency;

    float currentAmp;

    float fFmStart;
    float fFmStop;
    float fFmStep;
    float fFm;
    float fFmStopBand;   //Частота, которое делит весь диапазон частот на нижний и верхний
                                     // При переходе через нее идет переходный процесс, поэтому качать частоту
                                     // непрерывно невозможно. Если такого разделения нет, то установить значение
                                     // fFmStopBand > highestFrequency.
    int fmMode;
    int fmCounter;

    const float tFmMin;
    const float tFmMax;
    QTime tFmStart;

    int levelControlMode;





    int connectionTimerId;
    int FmTimerId;

    QSerialPort serialPort;
    QSerialPortInfo *serialPortInfo;

    QString logFileName;
    QFile logFile;

}; 
#endif // GENERATOR_H
