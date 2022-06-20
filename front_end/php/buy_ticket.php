<?php
    header("content-type:text/html;charset=utf-8");
    error_reporting(E_ALL &~E_NOTICE);
    $id = $_POST["username"];
    $train_id = $_POST["train_id"];
    $date = $_POST["date"];
    $start_station = $_POST["start_station"];
    $arrive_station = $_POST["arrive_station"];
    $ticket_num = $_POST["ticket_num"];
    $waitqueue = $_POST["waitqueue"];
    $command = "buy_ticket ".$id." ".$train_id." ".$start_station." ".$arrive_station." ".$ticket_num." ".$date." ".$waitqueue;
    $socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP);
    socket_connect($socket,"81.68.199.253",443);
    socket_write($socket,$command);
    $tmp_res=socket_read($socket,100000);
    echo "<script>alert('$tmp_res')</script>";
    echo "<script language='javascript' type='text/javascript'>";
    echo "window.location.href='http://81.68.199.253/index.html'";
    echo "</script>";
?>