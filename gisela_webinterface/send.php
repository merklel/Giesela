<?php



		//echo ("Befehl ".$_GET["value"]." gesendet!<br>\n");
		//echo(".GET["value"]");

		$command = $_GET["value"];


		$resv = "";
		$resvDaten = "";
		$timeout = 20;
		$timeoutDaten = 1;

		//----------------------------- Auswahl Socket-------------------------------------------------------
		//Python nutzt das Socket:
		//$socket = stream_socket_client('unix:///home/pi/gisela/gSockets/gSocketAction.sock', $errorno, $errorstr, $timeout);

		//C++ Version nutzt das Socket: 
		$socket = stream_socket_client('unix:///home/pi/gisela/gSockets/gSocketCpp.sock', $errorno, $errorstr, $timeout);
		//----------------------------- Auswahl Socket-------------------------------------------------------

		//echo error in socket communication
		//echo("socket: ".$errorstr."<br>\n");
		stream_set_timeout($socket, $timeout);
		echo("Socket: <br>\n". $Socket);

		//Nachricht an Server Socket senden
		//echo("Nachricht senden: " . $command . "<br>\n");
		if(!fwrite($socket, $command)) {
			echo("Fehler beim senden der Nachricht<br>\n");
		}else{
			echo("Nachrich ".$command." erfolgreich gesendet!<br\>\n");
		}
		
		//Antwort von server erhalten, nicht wirklich nötig
		//echo("Antwort empfangen:<br>\n");
		if (!($resv = fread($socket, 1024))) {
				//echo("Es konnte keine Nachricht empfangen werden<br>\n");
		} else {
				//echo($resv."<br>\n");
		}
		echo("Fertig<br>");  
		
		fclose($socket);	
		$command = 0;
		
		/*
		//Daten auslesen
		sleep(1);
		$socketDaten = stream_socket_client('unix:///home/pi/socket/socketDatenaustausch.sock', $errorno, $errorstr, $timeout);
		stream_set_timeout($socketDaten, $timeoutDaten);
				
		$command = "lesen\0";
		if(!fwrite($socketDaten, $command)) {
				echo("Fehler beim senden der Nachricht<br>\n");
		}
		echo("Status:<br>\n");
		if (!($resvDaten = fread($socketDaten, 1024))) {
				echo("Es konnte keine Nachricht empfangen werden<br>\n");
		} else {
				echo($resvDaten."<br>\n");
		}
		fclose($socketDaten);	
			
*/
		?>