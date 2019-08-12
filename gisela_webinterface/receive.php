<?php

		$resvDaten = "";
		$timeout = 20;
		$timeoutDaten = 1;

		//Daten auslesen
		//sleep(1);
		$socketDaten = stream_socket_client('unix:///home/pi/socket/socketDatenaustausch.sock', $errorno, $errorstr, $timeout);
		stream_set_timeout($socketDaten, $timeoutDaten);
				
		$command = "lesen\0";
		if(!fwrite($socketDaten, $command)) {
				echo("Fehler beim senden der Nachricht<br>\n");
		}
		
		echo("Status:<br>\n");
		if (!($resvDaten = fread($socketDaten, 50))) {
				echo("Es konnte keine Nachricht empfangen werden<br>\n");
		} else {
				echo($resvDaten);
		}
		fclose($socketDaten);	
			

?>