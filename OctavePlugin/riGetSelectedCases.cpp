#include <QtNetwork>
#include <octave/oct.h>
#include <octave/oct-map.h>

#include "riSettings.h"

void getSelectedCases(std::vector<qint64>& caseIds, std::vector<QString>& caseNames, std::vector<qint64>& caseTypes, std::vector<qint64>& caseGroupIds, const QString &hostName, quint16 port)
{
    QString serverName = hostName;
    quint16 serverPort = port;

    const int timeout = riOctavePlugin::timeOutMilliSecs;

    QTcpSocket socket;
    socket.connectToHost(serverName, serverPort);

    if (!socket.waitForConnected(timeout))
    {
        error((("Connection: ") + socket.errorString()).toLatin1().data());
        return;
    }

    // Create command and send it:

    QString command("GetSelectedCases");
    QByteArray cmdBytes = command.toLatin1();

    QDataStream socketStream(&socket);
    socketStream.setVersion(riOctavePlugin::qtDataStreamVersion);

    socketStream << (qint64)(cmdBytes.size());
    socket.write(cmdBytes);

    // Get response. First wait for the header

    while (socket.bytesAvailable() < (int)(sizeof(quint64)))
    {
        if (!socket.waitForReadyRead(timeout))
        {
            error((("Wating for header: ") + socket.errorString()).toLatin1().data());
            return;
        }
    }

    quint64 byteCount;
    socketStream >> byteCount;

    while (socket.bytesAvailable() < (int)(byteCount))
    {
        if (!socket.waitForReadyRead(timeout))
        {
            error((("Waiting for data: ") + socket.errorString()).toLatin1().data());
            return;
        }
        OCTAVE_QUIT;
    }

    quint64 selectionCount;
    socketStream >> selectionCount;

    qint64  caseId = -1;
    QString caseName;
    qint64  caseType = -1;
    qint64  caseGroupId = -1;

    for (size_t i = 0; i < selectionCount; i++)
    {
        socketStream >> caseId;
        socketStream >> caseName;
        socketStream >> caseType;
        socketStream >> caseGroupId;

        caseIds.push_back(caseId);
        caseNames.push_back(caseName);
        caseTypes.push_back(caseType);
        caseGroupIds.push_back(caseGroupId);
    }

    return;
}



DEFUN_DLD (riGetSelectedCases, args, nargout,
           "Usage:\n"
           "\n"
           "   riGetSelectedCases()\n"
           "\n"
           "Returns meta information for the Selected Case(s) in ResInsight.\n"
           )
{
    octave_value_list retval;

    int nargin = args.length ();
    if (nargin > 0)
    {
        error("riGetCurrentCase: Too many arguments, this function does not take any arguments.\n");
        print_usage();
    }
    else if (nargout != 1)
    {
        error("riGetCurrentCase: Wrong number of output arguments, this function requires one output argument.\n");
        print_usage();
    }
    else
    {
        std::vector<qint64>  caseIds;
        std::vector<QString> caseNames;
        std::vector<qint64>  caseTypes;
        std::vector<qint64>  caseGroupIds;

        getSelectedCases(caseIds, caseNames, caseTypes, caseGroupIds, "127.0.0.1", 40001);

        size_t caseCount = caseIds.size();

        if (caseCount != caseNames.size() ||
            caseCount != caseTypes.size() ||
            caseCount != caseGroupIds.size())
        {
            error("riGetCurrentCase: Inconsistent data received from ResInsight.\n");
        }
        else
        {
            // Create cells with N items for each field in the data structure
            
            Cell cellValuesA(caseCount, 1);
            Cell cellValuesB(caseCount, 1);
            Cell cellValuesC(caseCount, 1);
            Cell cellValuesD(caseCount, 1);

            for (size_t i = 0; i < caseCount; i++)
            {
                cellValuesA(i) = caseIds[i];
                cellValuesB(i) = caseNames[i].toLatin1().data();
                cellValuesC(i) = caseTypes[i];
                cellValuesD(i) = caseGroupIds[i];
            }

            // Build a map between the field name and field cell values
            
            Octave_map m;

            m.assign(riOctavePlugin::caseInfo_CaseId,      cellValuesA);
            m.assign(riOctavePlugin::caseInfo_CaseName,    cellValuesB);
            m.assign(riOctavePlugin::caseInfo_CaseType,    cellValuesC);
            m.assign(riOctavePlugin::caseInfo_CaseGroupId, cellValuesD);

            return octave_value(m);
        }
    }

    return retval;
}
