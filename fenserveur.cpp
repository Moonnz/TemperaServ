#include "fenserveur.h"

FenServeur::FenServeur()
{
    etatServeur = new QLabel;
    boutonQuitter = new QPushButton("Quitter");
    connect(boutonQuitter, SIGNAL(clicked(bool)), qApp, SLOT(quit()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(etatServeur);
    layout->addWidget(boutonQuitter);
    setLayout(layout);

    serveur = new QTcpServer(this);
    if (!serveur->listen(QHostAddress::Any, 99))
    {
        etatServeur->setText("Le serveur n'a pas pu être démarrer. Raison :<br />" + serveur->errorString());
    }
    else
    {
        etatServeur->setText("Le serveur a été démarré sur le port <strong>" + QString::number(serveur->serverPort()) + "</strong>.<br />Des clients peuvent maintenant se connecter.");
        connect(serveur, SIGNAL(newConnection()), this, SLOT(nouvelleConnexion()));
    }

    tailleMessage = 0;
}

void FenServeur::nouvelleConnexion()
{
    envoyerATous("<em> Un nouveau client vient de se connecter</em>");

    QTcpSocket *nouveauClient = serveur->nextPendingConnection();
    clients << nouveauClient;

    connect(nouveauClient, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(nouveauClient, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));
}

void FenServeur::donneesRecues()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;
    QDataStream in(socket);

    if (tailleMessage == 0)
    {
        if(socket->bytesAvailable() < (int)sizeof(quint16))
            return;

            in >> tailleMessage;
    }

    if (socket->bytesAvailable() < tailleMessage)
        return;

    QString message;
    in >> message;

    envoyerATous(message);

    tailleMessage = 0;
}

void FenServeur::deconnexionClient()
{
    envoyerATous("<em>Un client vient de se déconnecter</em>");

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;
    clients.removeOne(socket);

    socket->deleteLater();
}

void FenServeur::envoyerATous(const QString &message)
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));

    for(int i = 0; i < clients.size(); i++)
    {
        clients[i]->write(paquet);
    }
}
