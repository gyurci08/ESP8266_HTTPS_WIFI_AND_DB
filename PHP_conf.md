#DIRECTORY CHAIN
=============================================================================================================
./nodeMCU_Status/
	f			database_connection.php
	d 			testLED/
	f					database_read.php
	f					database_update.php

 #DATABASE CONNECTION 
=============================================================================================================

<?php
        class Database 
			{
				private static $dbName = 'database' ;
				private static $dbHost = 'localhost' ;
				private static $dbUsername = 'database_user';
				private static $dbUserPassword = 'secure_password';

				private static $cont  = null;

				public function __construct() 
					{
						die('Init function is not allowed');
					}

				public static function connect() 
					{
						// One connection through whole application
						if ( null == self::$cont ) 
							{
						
								try 
									{
									  self::$cont =  new PDO( "mysql:host=".self::$dbHost.";"."dbname=".self::$dbName, self::$dbUsername, self::$dbUserPassword);
									}
								catch(PDOException $e) 
									{
									  die($e->getMessage());
									}
							}
							  return self::$cont;
					}

				public static function disconnect() 
					{
						self::$cont = null;
					}
			}
?>



#DATABASE READ
=============================================================================================================

<?php
  include '../database_connection.php';

  $api_key_value = "this_is_your_secure_key";

  if (!empty($_POST)) 
	{
		$api_key =$_POST["API"];
		$id=$_POST["ID"];
		
		if ($api_key == $api_key_value)
        {
			$pdo = Database::connect();
			$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
			$sql = 'SELECT * FROM table WHERE ID=?';

			$q = $pdo->prepare($sql);
			$q->execute(array($id));
			$data = $q->fetch(PDO::FETCH_ASSOC);
			Database::disconnect();

			echo $data['Status'];
		}  
	}
?>



#DATABASE UPDATE 
=============================================================================================================

<?php
	require '../database_connection.php';

    $api_key_value = "this_is_your_secure_key";


    if (!empty($_POST)) 
	{
		$api_key =$_POST["API"];
        $Stat = $_POST['Status'];
        

    if ($api_key == $api_key_value)
        {
                // insert data
                 $pdo = Database::connect();
                 $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                 $sql = 'UPDATE table SET Status = ? WHERE ID = 1';
                 $q = $pdo->prepare($sql);
                 $q->execute(array($Stat));
                 Database::disconnect();
        }

	}
?>

