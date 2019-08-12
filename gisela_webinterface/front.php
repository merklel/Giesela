<?php

	$einstellungen = file("/home/pi/gisela/gSockets/config.txt");
	
	global $flag_time1;
	global $flag_time2;
	global $flag_time3;
	global $flag_waterReset;
	global $level_front;
	global $lastWater_front;
	
	for($i=0; $i < count($einstellungen); $i++){
		if (substr($einstellungen[$i], 0, 5) == "time1"){
			if (substr($einstellungen[$i], 6, 1) == "1"){
				$flag_time1 = 'checked';
			}else{
				$flag_time1 = "";
			}
			$time1 		= date_format(date_create(substr($einstellungen[$i], 8, 5)),'H:i');
			$gime1 		= substr($einstellungen[$i], 15, strlen($einstellungen[$i])-16);
			//echo $flag_time1 . " " . $time1 . "<br/>";
		}
		if (substr($einstellungen[$i], 0, 5) == "time2"){
			if (substr($einstellungen[$i], 6, 1) == "1"){
				$flag_time2 = 'checked';
			}else{
				$flag_time2 = "";
			}			
			$time2 		= date_format(date_create(substr($einstellungen[$i], 8, 5)),'H:i');
			$gime2		= substr($einstellungen[$i], 15, strlen($einstellungen[$i])-16);
			//echo $flag_time2 . " " . $time2. "<br/>";
		}
		if (substr($einstellungen[$i], 0, 5) == "time3"){
			if (substr($einstellungen[$i], 6, 1) == "1"){
				$flag_time3 = 'checked';
			}else{
				$flag_time3 = "";
			}
			$time3 		= date_format(date_create(substr($einstellungen[$i], 8, 5)),'H:i');
			$gime3 		= substr($einstellungen[$i], 15, strlen($einstellungen[$i])-16);
			//echo $flag_time3 . " " . $time3. "<br/>";
		}
		if (substr($einstellungen[$i], 0, 5) == "water"){
			$flag_waterReset = substr($einstellungen[$i], 11, 1);
			//echo "waterReset: " . $flag_waterReset;
		}
		if (substr($einstellungen[$i], 0, 5) == "level"){
			$level_front = substr($einstellungen[$i], 6, strlen($einstellungen[$i]) - 6);
		}
		if (substr($einstellungen[$i], 0, 5) == "lastW"){
			$lastWater_front = substr($einstellungen[$i], 10, strlen($einstellungen[$i]) - 10);
		}
	}


	function getLines($file){
	    $f = fopen($file, 'rb');
	    $lines = 0;

	    while (!feof($f)) {
	        $lines += substr_count(fread($f, 8192), "\n");
	    }

	    fclose($f);

	    return $lines;
	}

?>


<html>
	<head>
		<link href="designV2.css" type="text/css" rel="stylesheet">
		<title>Gi&szligela</title>
		<link rel="icon" href="favicon.png" type="image/png" />

		<script>
			function buttonSenden(str) {
			if (str.length == 0) {
				document.getElementById("status").innerHTML = "Nicht geklappt!";
				return;
			} else {
				var xmlhttp = new XMLHttpRequest();
				xmlhttp.onreadystatechange = function() {
					if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
						document.getElementById("status").innerHTML = xmlhttp.responseText;
					}
				};
				
				if (str == '2G'){
					document.getElementById("status").innerHTML = document.getElementById("colorpick").value;
					xmlhttp.open("GET", "send.php?value=" + str + document.getElementById("colorpick").value.substring(1,7), true);
					xmlhttp.send();
				}else{
					xmlhttp.open("GET", "send.php?value=" + str, true);
					xmlhttp.send();
				}
			}
		}
		</script>
	</head>
	<body>	
		<div class="container">
				<h1 id="ueberschrift"> Gie&szligela gie&szligt den Balkon</h1><br/><br/>
				
				<button class="button1" name="jetztGiesen" type="submit" value=1 onclick="buttonSenden('1')">jetzt Gie&szligen!</button>
				<!--
				<p>
				 Dauer: <input type="number" min="1" max="20" name="gime" value="<?php echo "10"; ?>"> Minuten -->
				
				
				
				<br/>
				<br/>
				
				<div class="status">
					<h2>Status</h2>
					<p><b>letzter Gießzeitpunkt:</b> <br/>
					<?php echo $lastWater_front; ?></p>
					<p><b>Wasserstand:</b> <?php echo round($level_front,1); ?>l</p>
				</div>
				
				<div class="einstellungen">
					<h2>Einstellungen</h2>
					<p>
					<form action="speicherEinstellungen.php" method = "post">
					<b>Automatisches Gie&szligen zu folgenden Zeitpunkten:</b>
					<p>
					Gie&szlig-Zeitpunkt 1:<input type="checkbox" name="flag_time1" <?php global $flag_time1; echo $flag_time1; ?>><input type="time" name="time1" value="<?php echo $time1; ?>"> Dauer: <input type="number" min="1" max="20" name="gime1" value="<?php echo $gime1; ?>"> Minuten<br/>
					Gie&szlig-Zeitpunkt 2:<input type="checkbox" name="flag_time2" <?php global $flag_time2; echo $flag_time2; ?>><input type="time" name="time2" value="<?php echo $time2; ?>"> Dauer: <input type="number" min="1" max="20" name="gime2" value="<?php echo $gime2; ?>"> Minuten<br/>
					Gie&szlig-Zeitpunkt 3:<input type="checkbox" name="flag_time3" <?php global $flag_time3; echo $flag_time3; ?>><input type="time" name="time3" value="<?php echo $time3; ?>"> Dauer: <input type="number" min="1" max="20" name="gime3" value="<?php echo $gime3; ?>"> Minuten<br/>
					<p>
					Wasserstand zur&uuml;cksetzen: <input type="checkbox" name="flag_resetWater"><br/>
					<input type="submit" value="speichern">
					<br/>
					<br/>
				</div>


				<div class="einstellungen">
					<h2>Log-File</h2>
					<div class=logfile>

						<?php 
						$n_lines = getLines("/home/pi/gisela/gSockets/log.txt");
						$max_n_lines = 100;
						echo "n Zeilen in Log: " . $n_lines . ". Angezeigt werden max. " . $max_n_lines;
						echo "<br/>";

						$ende_lines = 0;
						if ($n_lines > $max_n_lines){
							$ende_lines = $n_lines - $max_n_lines;
						}
						
						$myfile = fopen("/home/pi/gisela/gSockets/log.txt", "r"); 

						$c=0;

						while (!feof($myfile)) {
							if ($c >= $ende_lines){
						        echo fgets($myfile); 
						        echo "<br/>";
						    }else{
						    	fgets($myfile);
						    }
					        $c=$c+1;
					    }

						fclose($myfile); 

						?>
					</div>
				</div>


		</div>
	
	</body>
</html>