/**
 * Enclosure for the TROPIC01 USB Devkit
 * All measurements in mm
 * @author Ondřej Hruboš (github.com/hruboson)
 */
$fn = 100; // fragment number - level of detail

// board dimensions
board_length = 46;
board_width = 21;
board_thickness = 0.8;
max_component_height = 3.4; // tallest component
neck_width = 12;
neck_height = 11;
neck_start_x = board_length - neck_height;
board_to_neck_ratio_width = neck_width / board_width; // 60%
board_to_neck_ratio_height = neck_start_x / board_length;

// enclosure parameters
wall_thickness = 2;
clearance = 1.0;
total_height = board_thickness + max_component_height + clearance;
clip_height = 3;
clip_undercut = 0.8;
half_height = total_height/2 + 1;

// alignment pin diameter
pin_d = 4;
pin_d_hole = 4.2;

// helper constants
RERR = 0.01; // rounding error

module enclosure_half(upper = false, l = undef, w = undef, h = undef) {
    length = (l == undef) ? board_length + 2*wall_thickness : l;
    width = (l == undef) ? board_width + 2*wall_thickness : w;
    height = (l == undef) ? half_height : h;
    
    difference() {
        rounded_rect(
            length,
            width,
            height,
            radius = 2
        );
        
        translate([length*board_to_neck_ratio_height + RERR, -RERR, -RERR])
        cube([
            length*(1-board_to_neck_ratio_height) + RERR,
            width*(1-board_to_neck_ratio_width)/2 + RERR,
            height + RERR*2
        ]);
        translate([
                length*board_to_neck_ratio_height + RERR, 
                width-width*(1-board_to_neck_ratio_width)/2 + RERR, 
                -RERR
            ])
        cube([
            length*(1-board_to_neck_ratio_height) + RERR,
            width*(1-board_to_neck_ratio_width)/2 + RERR,
            height + RERR*2
        ]);
        
        translate([board_length+2*wall_thickness - 2, 0, -0.1])
        cube([4, width, height+3]);
    }

}

module rounded_rect(length, width, height, radius=2) {
    hull() {
        translate([radius, radius, 0])
            cylinder(r=radius, h=height);
        translate([length-radius, radius, 0])
            cylinder(r=radius, h=height);
        translate([radius, width-radius, 0])
            cylinder(r=radius, h=height);
        translate([length-radius, width-radius, 0])
            cylinder(r=radius, h=height);
    }
}

// main
translate([-board_length/2 - wall_thickness, -board_width/2 - wall_thickness, 0]) {
    difference() {
        enclosure_half(l = board_length + 2*wall_thickness, w = board_width + 2*wall_thickness, h = half_height);
        
        translate([wall_thickness, wall_thickness, 1])
        enclosure_half(upper = false, l = board_length, w = board_width, h = half_height);
    }
}

/*translate([board_length + wall_thickness + 10, 0, 0])
    enclosure_half(upper = true, l = board_length, w = board_width, h = half_height);*/