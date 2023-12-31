-- Active: 1699120710367@@127.0.0.1@3306@kingsoft


SHOW CREATE DATABASE Kingsoft;
SHOW CREATE TABLE users;


CREATE TABLE MeetingRooms (
    RoomID INT AUTO_INCREMENT PRIMARY KEY,
    RoomName VARCHAR(255) NOT NULL,
    Capacity INT NOT NULL
);
    --可以添加更多字段，例如会议室的位置、设备等
    --位置: Location VARCHAR(255),
    --设备: Equipment VARCHAR(255)

INSERT INTO MeetingRooms (RoomName, Capacity) 
VALUES 
    ('紫阳湖会议室', 14),
    ('后官湖会议室', 8),
    ('清潭湖会议室', 10),
    ('黄家湖会议室', 6),
    ('杨春湖会议室', 18),
    ('中山湖会议室', 150);

CREATE TABLE Users (
    UserID INT AUTO_INCREMENT PRIMARY KEY,
    EmployeeID VARCHAR(255) NOT NULL UNIQUE,
    Name_ VARCHAR(255) NOT NULL,
    Username VARCHAR(255) NOT NULL UNIQUE,
    Password_ VARCHAR(255) NOT NULL, 
    UserType ENUM('Business','Executive','Admin') NOT NULL
);

    -- 可以添加更多字段，例如部门、联系方式等
    -- 部门: Department VARCHAR(255),
    -- 联系方式: ContactInfo VARCHAR(255)

--INSERT INTO users(`UserID`,`EmployeeID`,`Name_`,`Username`,`Password_`,`UserType`) 
--               VALUES(1,'U202115512','洪炜豪','root','123456','Admin');
CREATE TABLE Bookings (
    BookingID INT AUTO_INCREMENT PRIMARY KEY,
    RoomID INT NOT NULL,
    UserID INT NOT NULL,
    Date_ DATE NOT NULL,
    TimeSlot ENUM('Morning', 'Afternoon') NOT NULL,
    Status_ ENUM('Pending', 'Approved', 'Rejected','Cancelled') NOT NULL,
    FOREIGN KEY (RoomID) REFERENCES MeetingRooms(RoomID),
    FOREIGN KEY (UserID) REFERENCES Users(UserID)
);
    -- 可以添加更多字段，例如预约备注、创建时间等
    -- 备注: Comment TEXT,
    -- 创建时间: CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP
