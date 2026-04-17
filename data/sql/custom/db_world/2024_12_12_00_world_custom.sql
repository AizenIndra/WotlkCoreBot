DELETE FROM `command` WHERE `name`='reload shop';
INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('reload shop', 3, 'Syntax: .reload shop'); 
