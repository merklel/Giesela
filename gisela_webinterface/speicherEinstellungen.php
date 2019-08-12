

<html>
	<head>
		<link href="designV2.css" type="text/css" rel="stylesheet">
		
		
		
		<script>
			function buttonSenden(str) {
			if (str.length == 0) {
				document.getElementById("saveStatus").innerHTML = "Nicht geklappt!";
				return;
			} else {
				var xmlhttp = new XMLHttpRequest();
				xmlhttp.onreadystatechange = function() {
					if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
						document.getElementById("saveStatus").innerHTML = xmlhttp.responseText;
						}
				};
				
			
				xmlhttp.open("GET", "send.php?value=" + str, true);
				xmlhttp.send();
				
			}
		}
		</script>
		
		
		
		
	</head>
	<body>
		<div class="container">
		<br/>
		<h2>speichere Einstellungen!</h2><br/>
		<p id="saveStatus">Noch kein Status erhalten</p>

		<br/>
		<?php
		//Formular einlesen
		$flag_time1 = $_POST["flag_time1"];
		$flag_time2 = $_POST["flag_time2"];
		$flag_time3 = $_POST["flag_time3"];

		$time1 = $_POST["time1"];
		$time2 = $_POST["time2"];
		$time3 = $_POST["time3"];

		$gime1 = $_POST["gime1"];
		$gime2 = $_POST["gime2"];
		$gime3 = $_POST["gime3"];

		// Checken ob bei times NIX eingegeben wurde, wenn ja auf 00:00 setzen und flag auf 0
		if ($time1 == ""){
			$time1 = "00:00";
			$flag_time1 = 'off';
		}
		if ($time2 == ""){
			$time2 = "00:00";
			$flag_time2 = 'off';
		}
		if ($time3 == ""){
			$time3 = "00:00";
			$flag_time3 = 'off';
		}
		
		$flag_resetWater = $_POST["flag_resetWater"];
		
		//Level und lastWater auslesen
		$einstellungen = file("/home/pi/gisela/gSockets/config.txt");
		$level = "0";
		$lastWater = "0";
		for($i=0; $i < count($einstellungen); $i++){
			if (substr($einstellungen[$i], 0, 5) == "level"){
				//echo substr($einstellungen[$i], 6, 2);
				$level 		= substr($einstellungen[$i], 6, strlen($einstellungen[$i]) - 6);
			}
			if (substr($einstellungen[$i], 0, 5) == "lastW"){
				//echo substr($einstellungen[$i], 6, 2);
				$lastWater 		= substr($einstellungen[$i], 10, strlen($einstellungen[$i]) - 10);
			}
		}
		
		// config Datei oeffnen
		$handle = fopen("/home/pi/gisela/gSockets/config.txt", "w");
	
		
		//Formular ausgegeben und Einstellungen abspeichern
		if($flag_time1 == 'on'){
			echo "Zeitslot 1 eingeschaltet. Zeitpunkt: " . $time1 . " Uhr<br/>";
			fwrite($handle, "time1 " . "1 " . $time1 . " ;" . $gime1 . "\n");
		}else{
			echo "Zeitslot 1 ausgeschaltet.". "<br/>";
			fwrite($handle, "time1 " . "0 " . $time1 . " ;" . $gime1 . "\n");
		}
		if($flag_time2 == 'on'){
			echo "Zeitslot 2 eingeschaltet. Zeitpunkt: ". $time2. " Uhr<br/>";
			fwrite($handle, "time2 " . "1 " . $time2 . " ;" . $gime2 . "\n");
		}else{
			echo "Zeitslot 2 ausgeschaltet.". "<br/>";
			fwrite($handle, "time2 " . "0 " . $time2 . " ;" . $gime2 . "\n");
		}
		if($flag_time3 == 'on'){
			echo "Zeitslot 3 eingeschaltet. Zeitpunkt: ". $time3. " Uhr<br/>";
			fwrite($handle, "time3 " . "1 " . $time3 . " ;" . $gime3 . "\n");
		}else{
			echo "Zeitslot 3 ausgeschaltet.". "<br/>";
			fwrite($handle, "time3 " . "0 " . $time3 . " ;" . $gime3 . "\n");
		}
		if($flag_resetWater == 'on'){
			echo "Wasserstand wird resettet". "<br/>";
			fwrite ($handle, "waterReset ". "1" . "\n");
		}else{
			fwrite ($handle, "waterReset ". "0" . "\n");
		}
		fwrite ($handle, "level " . $level . "\n");
		fwrite ($handle, "lastWater " . $lastWater . "\n");
		
		// Datei schlieﬂen
		fclose ($handle);
		
		// socket schreiben um zu sagen dass neue Einstellungen da sind
		echo '<script type="text/javascript">buttonSenden("2");</script>'; 
		?>
		
		<br/>
		<button class="button1" name="zurueck" type="submit" value=1 onclick="self.location.href='front.php'">zurueck</button>
		</div>
	</body>
</html>