<?php
    header("content-type:text/html;charset=utf-8");
    error_reporting(E_ALL &~E_NOTICE);
    $train_id = $_POST["train_id"];
    $date_traininform = $_POST["date_traininform"];
    $command = "query_train ".$train_id." ".$date_traininform;
    $socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP);
    socket_connect($socket,"81.68.199.253",443);
    socket_write($socket,$command);
    $tmp_res=socket_read($socket,100000);
    echo $tmp_res;
?>