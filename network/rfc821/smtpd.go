package main

import (
	"fmt"
	"net"
)

func main() {
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

	conn.Write([]byte("220 takoyaki SMTP Service Ready\r\n"))

	stage := 0
	messageBuf := make([]byte, 512)

	for {
		messageLen, _ := conn.Read(messageBuf)
		if messageLen <= 0 {
			return
		}
		message := string(messageBuf[:messageLen])
		command := string(message[:4])
		body := string(message[5:messageLen])

		if command == "HELO" && stage == 0 {
			senderDomain = body
			fmt.Printf("Accept HELO Domain:%s \n", senderDomain)
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
			conn.Write([]byte("221 Service closing transmission channel\r\n"))
			return
		} else if command == "NOOP" {
			conn.Write([]byte("250 OK\r\n"))
		} else if command == "RSET" && stage >= 1 {
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
			conn.Write([]byte("500 5.5.1 Command unrecognized\r\n"))
		}
	}

}
