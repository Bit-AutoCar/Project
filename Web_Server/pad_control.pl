#!/usr/bin/env perl

use v5.14;
use strict;
use warnings;

use Linux::Input::Joystick;
use LWP::UserAgent;
use YAML;

my $js = Linux::Input::Joystick->new('/dev/input/js0');

my $status = 'end';
my $fbstat = 'null'; #null, go, ba
my $rlstat = 'null'; #null, ra, le

my $ua = LWP::UserAgent->new;
my $server_endpoint = "http://192.168.1.80:3000/drive";

while (1) {
    my @event = $js->poll(0.01);
    my ( $number ,$type ,$value );

    if ($status ne 'start') {
        foreach (@event) {
            $number = $_->{number};
            $type   = $_->{type};
            $value  = $_->{value};
            if ($number eq '7' && $type eq '1'  && $value eq '1') {
                $status = 'start';
                say "입력이 시작되었습니다.";
            }
            elsif ($number eq '6' && $type eq '1'  && $value eq '1') {
                $status = 'end';
				say "입력이 중단되었습니다.";
            }
        }
    }
    else {
        foreach (@event) {
            $number = $_->{number};
            $type   = $_->{type};
            $value  = $_->{value};
            if ($type eq '1') {
                if ($number eq '6' && $type eq '1'  && $value eq '1') {
                    $status = 'end';
                    say "입력이 중단되었습니다.";
                    last;
                }
            }
            else {
				if ($number eq '1') {
					if ($fbstat eq 'null')
					{
						if ($value > 25000) {
							$ua->post($server_endpoint, { course => 'Back' });
							say ">> [Back]";
							$fbstat = 'ba';
						}
						elsif ($value < -25000) {
							$ua->post($server_endpoint, { course => 'Go' });
							say ">> [Go]";
							$fbstat = 'go';
						}
					}
					else {
						if (-5000 < $value && $value < 5000) {
							$ua->post($server_endpoint, { course => 'Stop' });
							say "   >> [GBSTOP]";
							$fbstat = 'null';
						}
						elsif ($value < -25000 && $fbstat eq 'ba') {
							$ua->post($server_endpoint, { course => 'Go' });
							say ">> [Go]";
							$fbstat = 'go';
						}
						elsif ($value > 25000 && $fbstat eq 'go') {
							$ua->post($server_endpoint, { course => 'Back' });
							say ">> [Back]";
							$fbstat = 'ba';
						}
					}

				}
				elsif ($number eq '3') {
					if ($rlstat eq 'null') {
						if ($value > 25000) {
							$ua->post($server_endpoint, { course => 'Right' });
							say "-- [Right]";
							$rlstat = 'ra';
						}
						elsif ($value < -25000) {
							$ua->post($server_endpoint, { course => 'Left' });
							say "-- [Left]";
							$rlstat = 'le';
						}
					}
					else {
						if (-5000 < $value && $value < 5000) {
							$ua->post($server_endpoint, { course => 'Stop' });
							say "   -- RLStop";
							$rlstat = 'null';
						}
						elsif ($value < -25000 && $rlstat eq 'ra') {
							$ua->post($server_endpoint, { course => 'Left' });
							say "-- [Left]";
							$rlstat = 'le';
						}
						elsif ($value > 25000 && $rlstat eq 'le') {
							$ua->post($server_endpoint, { course => 'Right' });
							say "-- [Right]";
							$rlstat = 'ra';
						}
					}
				}
            }
        }
    }
}
