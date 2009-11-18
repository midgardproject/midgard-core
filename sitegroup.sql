CREATE TABLE sitegroup (
  id smallint(5) unsigned DEFAULT '0' NOT NULL auto_increment,
  name varchar(255) DEFAULT '' NOT NULL,
  admingroup smallint(5) unsigned DEFAULT '0' NOT NULL,
  PRIMARY KEY (id)
);

ALTER TABLE article ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE element ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE file ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE grp ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE host ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE image ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE member ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE page ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE pageelement ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE person ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE preference ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE repligard ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE snippet ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE snippetdir ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE style ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;
ALTER TABLE topic ADD COLUMN
sitegroup smallint(5) unsigned DEFAULT '0' NOT NULL;

