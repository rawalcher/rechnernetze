# Ü 2.10 Einführung zu Wireshark

Wireshark (www.wireshark.org) ist ein für viele Plattformen verfügbares Tool zur Aufzeichnung von Netzwerkpaketen bzw. für deren weitere Analyse. Installieren Sie sich eine aktuelle Version dieser Software und machen Sie sich mit der Handhabung vertraut. Laden Sie dazu die Datei dump_protocols.pcap aus dem Moodle-Kurs auf Ihren Rechner und öffnen Sie diese Datei mit Wireshark.

- Erklären Sie kurz die Ausgabe des Programms bzw. die Funktionalität der einzelnen Ansichten.
    - Paketliste: Zeigt alle erfassten Pakete mit Infos wie Zeit, Quell-/Zieladresse und Protokoll.
    - Paketdetails: Zeigt Details des ausgewählten Pakets nach Protokollschichten (z.B. Ethernet, IP).
    - Paket-Bytes: Zeigt die Rohdaten des Pakets in Hex- und ASCII-Form.
- Welche Möglichkeiten zur Filterung bietet Ihnen Wireshark? Geben Sie ein Beispiel für eine Filterbedingung an.
    - Wireshark bietet viele verschiedene Möglichkeiten, die Pakete zu filtern. Einige Beispiele sind:
        - UDP, TCP-Pakete (Protokolle)
        - Filter nach IPv4, IPv6 oder MAC-Adressen
        - und vieles mehr.
- Starten Sie selbst eine Aufzeichnung (Capture) Ihres Netzwerkverkehrs. Erklären Sie die Schritte, die dazu notwendig sind.
    - Zuerst muss eines der Interfaces ausgewählt werden. Hier gibt es z.B. Bluetooth, WiFi, Ethernet oder VPN-Verbindungen.
    - Nach Auswahl eines Interfaces durch Doppelklick wird der Traffic erfasst.
- Wozu dient beim Aufzeichnen der Promiscuous Mode?
    - Der promiscuous Mode dient dazu, auch Pakete zu erfassen, die nicht direkt an den Host gerichtet sind.

# Ü 2.11 Protokolle mit Wireshark

Analysieren Sie die Datei `dump_protocols.pcap` mit Wireshark und beantworten Sie folgende Fragestellungen:
- Welche Netzwerkprotokolle werden in der Kommunikation verwendet?
    - ARP (Address Resolution Protocol)
    - ICMP (Internet Control Message Protocol)
    - TCP (Transmission Control Protocol)
    - DHCP (Dynamic Host Configuration Protocol)
    - DNS (Domain Name System)
    - HTTP (Hypertext Transfer Protocol)
- Ordnen Sie jedes der Protokolle einer Schicht im TCP/IP-Referenzmodell zu und stellen Sie die Hierarchie der einzelnen Protokolle graphisch dar.
    - Layer 2 (Data Link): ARP
    - Layer 3 (Network): ICMP
    - Layer 4 (Transport): TCP
    - Layer 7 (Application): DHCP, DNS, HTTP


# Ü 2.12 Wireshark – HTTP(S)

Laden Sie die Datei `dump_http.pcap` aus dem Moodle-Kurs herunter und analysieren Sie die Datei mit Wireshark. Die Datei beschreibt den Download einer recht trivialen Webseite durch einen HTTP-Client. Beantworten Sie folgende Fragestellungen:

- Welche Objekte werden vom Client via HTTP angefordert?
    - Der Client forder 3 Objekte mithilfe von GET an `\test\`, `\test\logo.gif` und `\test\TechnikErleben.png`
- Recherchieren Sie die Bedeutung der einzelnen Header-Felder bei den Anfragen bzw. Antworten des Servers.

1. ARP (Address Resolution Protocol)
    - HTYPE: Hardware-Typ (z.B. Ethernet).
    - PTYPE: Protokoll-Typ (z.B. IPv4).
    - HLEN/PLEN: Längen der Hardware-/Protokoll-Adressen.
    - Operation: Anfrage (1) oder Antwort (2).
    - SHA/SPA: MAC-/IP-Adresse des Absenders.
    - THA/TPA: MAC-/IP-Adresse des Ziels.

2. ICMP (Internet Control Message Protocol)
    - Type: Nachrichtentyp (z.B. Echo-Anfrage 8).
    - Code: Untertyp der Nachricht.
    - Checksum: Fehlerprüfung.
    - Rest of Header: Variiert je nach Typ (z.B. Identifier bei Echo).

3. TCP (Transmission Control Protocol)
    - Quell-/Zielport: Ports des Absenders und Empfängers.
    - Sequenznummer: Verfolgt die Reihenfolge der Daten.
    - Bestätigungsnummer: Erwartete nächste Sequenznummer.
    - Flags: Steuerflags (z.B. SYN, ACK).
    - Fenstergröße: Empfangsfenstergröße.
    - Checksum: Fehlerprüfung.

4. DHCP (Dynamic Host Configuration Protocol)
    - Op-Code: Anfrage (1) oder Antwort (2).
    - HTYPE/HLEN: Hardwaretyp/Länge der MAC-Adresse.
    - Hops: Anzahl der Relay-Stationen.
    - XID: Transaktions-ID.
    - Client IP: Client-IP (falls bekannt).
    - Your IP: Zugewiesene IP-Adresse.
    - Options: Zusätzliche Konfigurationen.

5. DNS (Domain Name System)
    - Transaktions-ID: Identifiziert die Anfrage.
    - Flags: Steuerbits (z.B. Anfrage oder Antwort).
    - Fragen/Antworten: Abgefragte Domains und deren Antworten.
    - Zusätzliche Felder: Infos wie Nameserver.

6. HTTP (Hypertext Transfer Protocol)
    - Methode (z.B. GET): Art der Anfrage.
    - URL: Angeforderte Resource.
    - HTTP-Version: Version des Protokolls.
    - Header-Felder: Weitere Infos (z.B. Host, User-Agent).
    
- Wie viele TCP-Verbindungen werden insgesamt aufgebaut? Wie unterscheidet sich das von dump_protocols.pcap?
    - Es wurden `68` Packets in total versendet, `6` davon `HTTP`
    - Für eine Connection muss `SYN` für die Init des Handshakes geschickt werden welche dann mit `SYN, ACK` bestätigt wird.
    - Bei Wireshark kann man mit `Statistics>Conversations>TCP` die Zustande gekommenen Connections einsehen welche `3` waren
- Bestimmen Sie, wie viele Bytes in jeder Verbindung ausgetauscht werden und wie lange die einzelnen Verbindungen bestehen.
    - Connection 1: Total Bytes `1kb`,  (A>B, B>A) = `476b, 760b`,  Relative Duration `0.3013`
    - Connection 2: Total Bytes `6kb`,  (A>B, B>A) = `616b, 5b`,    Relative Duration `0.3012`
    - Connection 3: Total Bytes `30kb`, (A>B, B>A) = `22b, 22b`,    Relative Duration `0.5022`

# Ü 2.13 Wireshark – HTTP(S)

Verbinden Sie sich mit `https://reference.dashif.org/dash.js/latest/samples/dash-if-reference-player/index.html` und analysieren Sie grob die Vorgänge beim Streamen eines Videos. Beantworten Sie folgende Fragestellungen:

- Welche Objekte werden vom Client via HTTP angefordert? Hinweis: nur jene beim Videostreaming, andere Objekte (z.B. HTML, Text, Bilder) können vernachlässigt werden.
  - Hauptsächlich werden `.m4v`- und `.m4a`-Dateien angefordert.
    - `.m4v`: `GET /akamai/bbb_30fps/bbb_30fps_3840x2160_12000k_31.m4v`
    - `.m4a`: `GET /akamai/bbb_30fps/bbb_a64k/bbb_a64k_32.m4a`
- Versuchen Sie die Verbindung über unterschiedliche Zugangsnetzwerke (z.B. LAN, WLAN, 3/4G sofern möglich) herzustellen und dokumentieren Sie allfällige Unterschiede.
  - Kann ich leider nicht durchführen, aber:
    - LAN: kürzere Ladezeiten, weniger Verbindungsabbrüche.
    - WLAN: längere Ladezeiten, mehr Verbindungsabbrüche.
    - 3G/4G/5G: höhere Latenz, je nach Netzabdeckung variable Qualität.
