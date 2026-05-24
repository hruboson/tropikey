#include <BOSL2/std.scad>
$fn = 100;

/**
 * I'm sorry this code is so ugly, I swear I will refactor this ... later ...
 */

usb_c_width = 8.2;
usb_c_neck_length = 2;

joint_depth = 1;
negative_height = 10;

module board_reference(wt = 0, h = 3){
    // scale to real size (board width=20)
    linear_extrude(h)
    offset(wt)
    scale([0.192, 0.192, 0.192])
    color(c = [0.5, 0.5, 0.5], alpha = 1)
    import("reference/enclosure.svg", convexity=2, center=true);
}

module joint(ft, wt){
    difference(){
        translate([0,0,ft+joint_depth])
        board_reference(wt = 1.5 - 0.1, h = 2);
            
        translate([0,0,ft+joint_depth/2])
        board_reference(wt = 0.5 + 0.1, h = 4);
            
        translate([0,-20.5-(2*wt+usb_c_neck_length),negative_height/2+ft])
        cube([usb_c_width,4*wt+usb_c_neck_length,negative_height], center = true);
                          
        translate([0,-25,0])
        cube([20,5,10], center = true);
    }
}

module usb_c_neck(wt, ft, h){
    difference(){
        translate([0,-20.5-(wt+usb_c_neck_length),0])
        linear_extrude(h+ft)
        offset(wt)
        square([usb_c_width,wt+usb_c_neck_length], center = true);
        
        translate([0,-20.5-(2*wt+usb_c_neck_length),negative_height/2+ft])
        cube([usb_c_width,4*wt+usb_c_neck_length,negative_height], center = true);
        
        translate([0,-20.5-wt/2,negative_height/2+ft])
        cube([usb_c_width+2*wt,wt*2,negative_height], center = true);
        
        translate([0,-21.5,negative_height/2 + ft-0.5])
        cube([12,3,negative_height+0.2], center = true);
    }
}

module board_half(wt = 2, ft = 1, h = 1.5, top = false){   
    
    if(!top){
        joint(ft = ft, wt = wt);
    }

    usb_c_neck(wt, ft, h);
    
    difference(){
        if(top){
            difference(){
                board_reference(wt = wt, h = ft+h);
                
                translate([0,0,-joint_depth])
                joint(ft = ft, wt = wt);
            }
        }else{
            board_reference(wt = wt, h = ft+h);
        }
        
        up(ft)
        board_reference();
        
        
        color("blue")
        translate([0,-20.5-(2*wt+usb_c_neck_length),negative_height/2+ft])
        cube([usb_c_width,4*wt+usb_c_neck_length,negative_height], center = true);
        
        if(!top){
            color("red")
            translate([0,-21.5,negative_height/2 + ft-0.5])
            cube([12,3,negative_height+0.2], center = true);
        }
        
        if(top){
            color("purple")
            translate([0,-9,0])
            cube([6,3.5,10], center = true);
        }
    }
}

board_half();

translate([30,0,0])
board_half(h = 1, top = true);