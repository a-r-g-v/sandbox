package main

import (
	"fmt"
	log "github.com/Sirupsen/logrus"
	"net"
	"strings"
)

func main() {
	log.SetLevel(log.DebugLevel)
	service := ":25"
	tcpAddr, _ := net.ResolveTCPAddr("tcp", service)
	listner, _ := net.ListenTCP("tcp", tcpAddr)
	fmt.Println("listen tcp/25")
	for {
		conn, err := listner.Accept()
		if err != nil {
			continue
		}

		go session(conn)
	}
}

func session(conn net.Conn) {
	defer conn.Close()
	var senderDomain string
	var body string
	addr := conn.RemoteAddr().String()

	conn.Write([]byte("220 takoyaki SMTP Service Ready\r\n"))
	log.WithFields(log.Fields{"level": "info", "state": "ACCEPT", "addr": addr}).Info("Accept")

	stage := 0
	messageBuf := make([]byte, 512)

	for {
		messageLen, _ := conn.Read(messageBuf)
		log.WithFields(log.Fields{"level": "debug", "addr": addr, "domain": senderDomain, "stage": stage, "messageLen": messageLen}).Debug("Read")
		if messageLen <= 0 {
			return
		}

		message := string(messageBuf[:messageLen])

		if messageLen <= 4 {
			log.WithFields(log.Fields{"level": "info", "state": "unrecognized", "addr": addr, "domain": senderDomain, "stage": stage}).Info("unrecognized: " + strings.Trim(message, "\n"))
			conn.Write([]byte("500 5.5.1 Command unrecognized\r\n"))
			continue
		}

		command := string(message[:4])
		body = strings.Trim(string(message[5:messageLen]), "\n")
		log.WithFields(log.Fields{"level": "debug", "addr": addr, "domain": senderDomain, "stage": stage, "messageLen": messageLen, "command": command, "body": body}).Debug("Generate")

		if command == "HELO" && stage == 0 {
			senderDomain = body
			log.WithFields(log.Fields{"level": "info", "state": "ACCEPT", "addr": addr, "domain": senderDomain, "stage": stage}).Info("Accept HELO")
			stage = 1
			conn.Write([]byte("250 pleased to meet you\r\n"))
		} else if command == "MAIL" && stage == 1 {
			stage = 2
			conn.Write([]byte("250 OK\r\n"))
		} else if command == "RCPT" && stage == 2 {
			stage = 3
			conn.Write([]byte("250 OK\r\n"))
		} else if command == "DATA" && stage == 3 {
			stage = 1
			conn.Write([]byte("354 Start mail input; end with <CRLF>.<CRLF>\r\n"))
			conn.Write([]byte("250 OK\r\n"))
		} else if command == "QUIT" {
			log.WithFields(log.Fields{"level": "info", "state": "QUIT", "addr": addr, "domain": senderDomain, "stage": stage}).Info("Accept QUIT")
			conn.Write([]byte("221 Service closing transmission channel\r\n"))
			return
		} else if command == "NOOP" {
			conn.Write([]byte("250 OK\r\n"))
		} else if command == "RSET" && stage >= 1 {
			log.WithFields(log.Fields{"level": "info", "state": "reset", "addr": addr, "domain": senderDomain, "stage": stage}).Info("accept rset")
			stage = 1
			conn.Write([]byte("250 OK\r\n"))
		} else if command == "VRFY" && stage == 1 {
			conn.Write([]byte("550 Access Denied to You.\r\n"))
		} else if command == "EXPN" && stage == 1 {
			conn.Write([]byte("550 Access Denied to You.\r\n"))
		} else if command == "TURN" && stage == 1 {
			conn.Write([]byte("502\r\n"))
		} else if command == "HELP" {
			conn.Write([]byte("214-Command supported\r\n"))
			conn.Write([]byte("214 HELO MAIL RCPT DATA NOOP QUIT REST HELP\r\n"))
		} else {
			log.WithFields(log.Fields{"level": "info", "state": "unrecognized", "addr": addr, "domain": senderDomain, "stage": stage}).Info("unrecognized: " + strings.Trim(message, "\n"))
			conn.Write([]byte("500 5.5.1 Command unrecognized\r\n"))
		}
	}

}
