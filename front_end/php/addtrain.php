<?php
    header("content-type:text/html;charset=utf-8");
    error_reporting(E_ALL &~E_NOTICE);
    // echo "here";
    $id = $_POST["train_id"];
    $seat_num = $_POST["seat_num"];
    $station_num = $_POST["station_num"];
    $start_sale_date = $_POST["start_sale_date"];
    $end_sale_date = $_POST["end_sale_date"];
    $start_time = $_POST["start_time"];
    $type = $_POST["type"];
    
    $station1 = $_POST["station1"];
    $station2 = $_POST["station2"];
    $station3 = $_POST["station3"];
    $station4 = $_POST["station4"];
    $station5 = $_POST["station5"];
    
    $travel_time_1 = $_POST["travel_time_1"];
    $travel_time_2 = $_POST["travel_time_2"];
    $travel_time_3 = $_POST["travel_time_3"];
    $travel_time_4 = $_POST["travel_time_4"];
    
    $price2 = $_POST["price2"];
    $price3 = $_POST["price3"];
    $price4 = $_POST["price4"];
    $price5 = $_POST["price5"];
    
    $stop_time_2 = $_POST["stop_time_2"];
    $stop_time_3 = $_POST["stop_time_3"];
    $stop_time_4 = $_POST["stop_time_4"];
    
    $command = "add_train ".$id." ".$seat_num." ".$station_num." ".$start_sale_date." ".$end_sale_date." ".$start_time." ".$type." ".$station1."|".$station2."|".$station3."|".$station4."|".$station5." ".$travel_time_1."|".$travel_time_2."|".$travel_time_3."|".$travel_time_4." ".$price2."|".$price3."|".$price4."|".$price5." ".$stop_time_2."|".$stop_time_3."|".$stop_time_4;
    $socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP);
    socket_connect($socket,"81.68.199.253",443);
    socket_write($socket,$command);
    $tmp_res=socket_read($socket,100000);
    echo "<script>alert('$tmp_res')</script>";
    echo "<script language='javascript' type='text/javascript'>";
    echo "window.location.href='http://81.68.199.253/index.html'";
    echo "</script>";
?>