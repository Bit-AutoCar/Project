#!/usr/bin/env perl

use v5.14;
use utf8;
use strict;
use warnings;

use Device::SerialPort::Arduino;
use Linux::Input::Joystick;
use YAML;

binmode STDOUT, ':encoding(UTF-8)';

my $js = Linux::Input::Joystick->new('/dev/input/js0');
my $Arduino = Device::SerialPort::Arduino->new(
				port => '/dev/ttyAMA0',
				baudrate => 115200,
			);

my $status = 'end';
my $fbstat = 'null'; #null, go, ba
my $rlstat = 'null'; #null, ra, le
my $GBgear = '1';

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
            if ($type eq '1' && $value eq '1') {
				if ($number eq '0') {
						$Arduino->communicate(chr(105));
				}
				elsif ($number eq '1') {
						$Arduino->communicate(chr(102));
				}
				elsif ($number eq '2') {
						$Arduino->communicate(chr(101));
				}
				elsif ($number eq '3') {
						$Arduino->communicate(chr(104));
				}
				elsif ($number eq '4') {
					unless ($GBgear eq '1') {
						$GBgear--;
						$Arduino->communicate(chr($GBgear+96));
					}
				}
				elsif ($number eq '5') {
					unless ($GBgear eq '4') {
						$GBgear++;
						$Arduino->communicate(chr($GBgear+96));
					}
				}
                elsif ($number eq '6') {
                    $status = 'end';
                    say "입력이 중단되었습니다.";
                    last;
                }

				say chr($GBgear+96).": $number : $type : $value";
            }
            else {
				if ($number eq '1') {
					if ($fbstat eq 'null')
					{
						if ($value > 25000) {
							$Arduino->communicate(2);#Back
							$fbstat = 'ba';
						}
						elsif ($value < -25000) {
							$Arduino->communicate(1);#Go
							$fbstat = 'go';
							say "go---debug";
						}
					}
					else {
						if (-5000 < $value && $value < 5000) {
							$Arduino->communicate(8);#GBStop
							$fbstat = 'null';
						}
						elsif ($value < -25000 && $fbstat eq 'ba') {
							$Arduino->communicate(1);#Go
							say "go---debug";
							$fbstat = 'go';
						}
						elsif ($value > 25000 && $fbstat eq 'go') {
							$Arduino->communicate(2);#Back
							$fbstat = 'ba';
						}
					}

				}
				elsif ($number eq '3') {
					if ($rlstat eq 'null') {
						if ($value > 25000) {
							$Arduino->communicate(6);#Right
							$rlstat = 'ra';
						}
						elsif ($value < -25000) {
							$Arduino->communicate(4);#Left
							$rlstat = 'le';
						}
					}
					else {
						if (-5000 < $value && $value < 5000) {
							$Arduino->communicate(7);#LRSTop
							$rlstat = 'null';
						}
						elsif ($value < -25000 && $rlstat eq 'ra') {
							$Arduino->communicate(4);#Left
							$rlstat = 'le';
						}
						elsif ($value > 25000 && $rlstat eq 'le') {
							$Arduino->communicate(6);#Right
							$rlstat = 'ra';
						}
					}
				}
            }
        }
    }
}
