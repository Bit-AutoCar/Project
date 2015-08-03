#!/usr/bin/env perl 

use v5.14;
use strict;
use warnings;
use IO::Socket::INET;
use LWP::UserAgent;

$| = 1;

my ($socket,$client_socket);
my ($peeraddress,$peerport);

$socket = new IO::Socket::INET (
        LocalHost => '192.168.1.3',
        LocalPort => '3000',
        Proto     => 'tcp',
        Listen    => 5,
        Reuse     => 1
) or die "ERROR in Socket Creation : $!\n";

print "SERVER Waiting for client connection on port 3000\n";

while(1)
{
    $client_socket = $socket->accept();

    if ($client_socket) {
        say $client_socket;
        my $sockport = $client_socket->sockport();
        my $sockhost = $client_socket->sockhost();

        my $peerport = $client_socket->peerport();
        my $peerhost = $client_socket->peerhost();

        say "p_port : $peerport\n
             p_host : $peerhost\n";

        my $pid = fork;
        if (!defined $pid) {
            die "Cannot fork: $!";
        }
        elsif ($pid == 0) {
#PIN MODE
            my $data;
            while (1) {
                say "[Before Wait]";
                $client_socket->recv($data,1024);
                say "[After Wait]"."\n";

                if ($data) {
                    say "[$data]"."\n";
                }
                else {
                    say "[$data]";
                    exit;
                }
                $data = '';
                say "[End Wait]";

#                my $ua = LWP::UserAgent->new;
#                my $server_endpoint = "http://192.168.1.3:3001/list";
#
## set custom HTTP request header fields
#                my $req = HTTP::Request->new(POST => $server_endpoint);
#                $req->header('content-type' => 'application/json');
#                $req->header('x-auth-token' => 'kfksj48sdfj4jd9d');
#
## add POST data to HTTP request body
#                my $post_data = "{ name: $data }";
#                $req->content($post_data);
            }
        }
        else {
            say "Parent process, waiting for child...";
            my $data = "Return";
            print $client_socket "$data\n";

            $data = <$client_socket>;
            print "Received from Client : $data\n";
        }
        say "Parent process after child has finished";
    }
    else {
        say "DEBUG";
    }

}

$socket->close();
