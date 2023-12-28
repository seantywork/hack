CREATE USER 'runpayload'@'%' IDENTIFIED BY 'youdonthavetoknow';

GRANT ALL PRIVILEGES ON *.* TO 'runpayload'@'%';

CREATE DATABASE runpayload;

USE runpayload;

CREATE TABLE payload_progress(
    p_index INT NOT NULL AUTO_INCREMENT,
    p_email VARCHAR(255),
    p_owner VARCHAR(255),
    submit_lang VARCHAR(255),
    submit_version VARCHAR(255),
    submit_code TEXT,
    submit_input TEXT,
    creation_time TIMESTAMP,
    effective TINYINT,
    verified TINYINT,
    jam_deployment_name VARCHAR(255),
    jam_deployment_yaml TEXT,
    jam_configmap_name VARCHAR(255),
    exec_start_time TIMESTAMP,
    log_resource TEXT,
    log_stdout TEXT,
    log_stderr TEXT,
    illegal_flag VARCHAR(255),
    termination_time TIMESTAMP,
    PRIMARY KEY (p_index)
);


COMMIT;