/**
 * Enclosure for the TROPIC01 USB Devkit
 * All measurements in mm
 * @author Ondřej Hruboš (github.com/hruboson)
 */
$fn = 100; // fragment number - level of detail

// board dimensions
thick_part_length = 34; // the main part of the board without neck
board_width = 20;
floor_thickness = 0.6;
max_component_height = 2.5;
neck_width = 12;
neck_length = 14;
usb_c_width = 8.2;

// enclosure parameters
wall_thickness = 3;
clearance = 1.0;
total_height = floor_thickness + max_component_height + clearance;
clip_height = 3;
clip_undercut = 0.8;
half_height = total_height / 2;

// alignment pin
pin_width = 1.5;
pin_depth = half_height;
pin_outwards_width = pin_width - 0.07; // this might need adjusting
pin_outwards_depth = pin_depth - 0.5;

// helper constants
RERR = 0.01; // rounding error

/**
 * Construct one half of the enclosure
 * @param {bool} [upper=false] - upper or lower case, changes the joints
 * @param {real} l - length
 * @param {real} w - width
 * @param {real} h - height
 * @param {bool} [base=true] - false to not generate any joints
 */
module enclosure_half(upper = false, l = undef, w = undef, h = undef, base = true) {
    length = (l == undef) ? thick_part_length + 2*wall_thickness : l;
    width = (w == undef) ? thick_part_length + 2*wall_thickness : w;
    height = (h == undef) ? half_height : h;
    
    difference() {
        rounded_rect(
            length,
            width,
            height,
            radius = 2
        );
                    
        // cutoff at the neck top
        translate([thick_part_length, (width/2-neck_width/2), height/2 - 2.5*RERR]) // I have no idea why the -2.5*RERR works
        cube([wall_thickness*2 + 2*RERR, neck_width, height]);
        
        // joints
        if(!upper && base){        
            translate([wall_thickness*0.5, width / 2, height])
            cube([pin_width, width*0.6, pin_depth], center = true);
            
            translate([length / 2, width - wall_thickness*0.5, height])
            rotate(90)
            cube([pin_width, width*0.6, pin_depth], center = true);
            
            translate([length / 2, wall_thickness*0.5, height])
            rotate(90)
            cube([pin_width, width*0.6, pin_depth], center = true);
        }
        
        // hole for button
        if(upper && base){
            translate([length*0.88,width/2, -RERR])
            cube([4, width*0.3, half_height], center = true);      
        }
    }
    
    
    // upper half
    if(upper && base){        
        translate([wall_thickness*0.5, width / 2, height])
        cube([pin_outwards_width, width*0.6, pin_outwards_depth], center = true);
           
        translate([length / 2, width - wall_thickness*0.5, height])
        rotate(90)
        cube([pin_outwards_width, width*0.6, pin_outwards_depth], center = true);
        
        translate([length / 2, wall_thickness*0.5, height])
        rotate(90)
        cube([pin_outwards_width, width*0.6, pin_outwards_depth], center = true);
    }
}

module enclosure_half_neck(upper = false, l = undef, w = undef, h = undef, base = true) {
    length = (l == undef) ? neck_length + 2*wall_thickness : l;
    width = (w == undef) ? neck_width + 2*wall_thickness : w;
    height = (h == undef) ? half_height : h;
    
    difference() {
        cube([length, width, height]);
        
        if(!upper && base){
            translate([length*0.6, width-wall_thickness/2, height])
            rotate(90)
            cube([pin_width, width*0.5, pin_depth], center = true);
            
            translate([length*0.6, wall_thickness/2, height])
            rotate(90)
            cube([pin_width, width*0.5, pin_depth], center = true);
        }
    }
    
    if(upper && base){
        translate([length*0.6, width-wall_thickness/2, height])
        rotate(90)
        cube([pin_width, width*0.5, pin_depth], center = true);
            
        translate([length*0.6, wall_thickness/2, height])
        rotate(90)
        cube([pin_width, width*0.5, pin_depth], center = true);
    }
}

/**
 * Create a rectangle with two rounded corners and two square corners and uniform height
 * @param {real} length - length of the rectangle along X-axis
 * @param {real} width - width of the rectangle along Y-axis
 * @param {real} height - height of the extruded shape along Z-axis
 * @param {real} [radius=2] - corner radius in mm
 */
module rounded_rect(length, width, height, radius=2) {
    hull() {
        translate([radius, radius, 0])
            cylinder(r=radius, h=height);
        translate([radius, width-radius, 0])
            cylinder(r=radius, h=height);
        
        translate([length-radius, 0, 0])
            cube([radius, radius, height], 0);
        translate([length-radius, width-radius, 0])
            cube([radius, radius, height], 0);
    }
}

//////////////////////
//      MAIN        //
//////////////////////

translate([-thick_part_length/2 - wall_thickness, -board_width/2 - wall_thickness, 0]) {
    difference() {
        enclosure_half(l = thick_part_length + 2*wall_thickness, w = board_width + 2*wall_thickness, h = half_height);
        
        translate([wall_thickness, wall_thickness, 1])
        enclosure_half(l = thick_part_length, w = board_width, h = half_height, base = false);
    }
    
    translate([thick_part_length + 2*wall_thickness, board_width/2 - 2*wall_thickness, 0])
    difference() {
        enclosure_half_neck(l = neck_length, w = neck_width+2*wall_thickness, h = half_height);
        
        translate([-RERR, wall_thickness, 1])
        enclosure_half_neck(l = neck_length + 2*wall_thickness + RERR, w = neck_width, h = half_height, base = false);
    }
}

translate([-thick_part_length/2 - wall_thickness + 60, -board_width/2 - wall_thickness, 0]) {
    difference() {
        enclosure_half(upper = true, l = thick_part_length + 2*wall_thickness, w = board_width + 2*wall_thickness, h = half_height);
        
        translate([wall_thickness, wall_thickness, 1])
        enclosure_half(upper = true, l = thick_part_length, w = board_width, h = half_height, base = false);
    }
    
    translate([thick_part_length + 2*wall_thickness, board_width/2 - 2*wall_thickness, 0])
    difference() {
        enclosure_half_neck(upper = true, l = neck_length, w = neck_width+2*wall_thickness, h = half_height);
        
        translate([-RERR, wall_thickness, 1])
        enclosure_half_neck(upper = true, l = neck_length + 2*wall_thickness + RERR, w = neck_width, h = half_height, base = false);
    }
}